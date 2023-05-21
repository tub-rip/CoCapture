import argparse
import glob
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

import cv2
import numpy as np
import tqdm
from metavision_sdk_core import BaseFrameGenerationAlgorithm
from PIL import Image

from event_readers import FixedDurationFramerate, FixedSizeFramerate


def get_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "dir",
        type=Path,
        help="Directory containing two folders: prophesee_0 and prophesee_1",
    )
    parser.add_argument(
        "--duration", type=float, default=None, help="Accumulation time [s]"
    )
    parser.add_argument(
        "--events", type=int, default=None, help="Number of events per frame"
    )
    parser.add_argument(
        "--save_video",
        action="store_true",
        help="Save the event representations to video",
    )
    parser.add_argument(
        "--fps", type=int, default=30, help="Frames per second of the saved video"
    )
    parser.add_argument(
        "--display", action="store_true", help="Display the generated representations"
    )
    parser.add_argument(
        "--save_frames_dir",
        type=Path,
        default=None,
        help="Folder where to save video frames as png (if not specified, frames are not saved)",
    )
    parser.add_argument(
        "--video_path", type=Path, default=None, help="File path of output video file"
    )

    return parser


def load_files(input_dir):
    files = [
        os.path.join(root, name)
        for root, _, files in os.walk(input_dir)
        for name in files
        if name.endswith(".raw")
    ]
    assert len(files) == 2
    print("Found files:", files)
    return files


def init_readers(args, files):
    if args.duration is None and args.events is not None:
        return FixedSizeFramerate(files[0], args.events, args.fps), FixedSizeFramerate(
            files[0], args.events, args.fps
        )

    elif args.duration is not None and args.events is not None:
        raise RuntimeError(
            "Both events and duration cannot be specified simultaneously. Please select one."
        )

    elif args.duration is None and args.events is None:
        print(
            "Neither accumulation time nor events argument specified. Will use default 0.01 [s] accumulation time."
        )
        args.duration = 0.01

    return FixedDurationFramerate(
        files[0], args.duration, args.fps
    ), FixedDurationFramerate(files[1], args.duration, args.fps)


def main():
    args = get_parser().parse_args()
    files = load_files(args.dir)
    fname = args.dir / Path(files[0]).with_suffix("").name
    if args.video_path is not None:
        data_filename = args.video_path / fname.name
        if not args.video_path.exists():
            args.video_path.mkdir(parents=True)
    else:
        data_filename = fname

    event_reader0, event_reader1 = init_readers(args, files)
    height, width = event_reader0.raw_reader.get_size()

    if args.display:
        print("Press spacebar to stop/resume playing and q to quit.")

    with tempfile.TemporaryDirectory() as temp_dir:
        path_to = Path(temp_dir)
        if args.save_frames_dir is not None:
            image_output_path = args.save_frames_dir
            if image_output_path.exists():
                if image_output_path.is_dir():
                    string = input(
                        "The folder %s already exists. Do you want to overwrite it? Type y or yes to continue, and any other to continue without saving frames.\n"
                        % image_output_path
                    )
                    if string.lower() == "y" or string.lower() == "yes":
                        print("OK, folder will be overwritten.")
                        shutil.rmtree(image_output_path)
                    else:
                        print(
                            "OK, frames will not be saved and folder is left untouched.\n"
                        )
                        args.save_frames_dir = None
            image_output_path.mkdir(exist_ok=True, parents=True)

        idx = 0
        for events_cam0, events_cam1 in tqdm.tqdm(zip(event_reader0, event_reader1)):
            frame = np.zeros((height, width, 3), dtype=np.uint8)
            BaseFrameGenerationAlgorithm.generate_frame(events_cam0, frame)
            frame1 = np.zeros((height, width, 3), dtype=np.uint8)
            BaseFrameGenerationAlgorithm.generate_frame(events_cam1, frame1)
            stacked_frame = np.hstack((frame, frame1))
            if args.display:
                cv2.imshow("EVS Stereo Pair", stacked_frame)
                k = cv2.waitKey(int(1 / args.fps * 100))
                if k == ord("q"):
                    sys.exit()
                elif k == ord(" "):
                    while cv2.waitKey() != ord(" "):
                        pass

            if args.save_video and event_reader0.at_framerate():
                video_frame = Image.fromarray(stacked_frame)
                filename = path_to / ("%09d.png" % idx)
                video_frame.save(filename)
                idx += 1

        if args.save_video:
            command_mp4 = (
                "ffmpeg -v 0"
                + " -r {}".format(args.fps)
                + " -i "
                + str(path_to / "%09d.png")
                + " -vcodec mpeg4 -q:v 8 -pix_fmt yuv420p "
            )
            subprocess.run(
                command_mp4 + str(data_filename) + ".mp4", shell=True, check=True
            )
            print("Video generated at: " + str(data_filename) + ".mp4")

        # store image in folder if required
        if args.save_frames_dir is not None:
            for filename in glob.glob(os.path.join(temp_dir, "*.png*")):
                shutil.copy(filename, image_output_path)
            print("Images stored at: {}".format(image_output_path))


if __name__ == "__main__":
    main()
