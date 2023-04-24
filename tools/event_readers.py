from metavision_core.event_io.raw_reader import RawReader

MAX_RELATIVE_ERROR = 0.3


class FixedSizeFramerate:
    """Reads events from a prophesee .raw file, and packages the events into non-overlapping event windows, each
    containing a number of events as close as possible to the desired one such that resulting windows match a desired
    framerate."""

    def __init__(self, path, n, framerate=1):
        self.raw_reader = RawReader(path)
        self.interval = n
        # go once through the file to get last timestamp
        while not self.raw_reader.is_done():
            try:
                events = self.raw_reader.load_n_events(self.interval)
            except:
                break

        last_ts = events["t"][-1]
        delta_t = 1 / float(framerate) * 1000000  # microsecs

        self.count_events_array = []
        self.save_flags = []
        self.raw_reader.reset()
        frame_counter = 1
        while delta_t * frame_counter < last_ts:
            start_idx = self.raw_reader.current_event_index()
            self.raw_reader.seek_time(delta_t * frame_counter)
            end_idx = self.raw_reader.current_event_index()
            batches = round((end_idx - start_idx) / self.interval)
            interval = (
                (end_idx - start_idx)
                if batches == 0
                else round((end_idx - start_idx) / batches)
            )
            if abs(interval - self.interval) / self.interval > MAX_RELATIVE_ERROR:
                print(
                    "WARNING: Can't produce desired framerate with specified interval."
                    + " Desired: {} events, In use: {} events".format(
                        self.interval, interval
                    )
                )

            batches = max(1, batches)
            self.count_events_array += [
                interval + (end_idx - start_idx) - batches * interval
            ]
            self.count_events_array += [interval] * (batches - 1)
            assert sum(self.count_events_array[-batches:]) == (end_idx - start_idx)
            self.save_flags += [False] * (batches - 1)
            self.save_flags += [True]
            frame_counter += 1

        self.counter = 0

        self.raw_reader.reset()

    def __iter__(self):
        return self

    def __next__(self):
        if not self.raw_reader.is_done() and self.counter < len(
            self.count_events_array
        ):
            events = self.raw_reader.load_n_events(
                self.count_events_array[self.counter]
            )
            self.counter += 1
            return events

        raise StopIteration

    def at_framerate(self):
        return self.save_flags[self.counter - 1]


class FixedDurationFramerate:
    """Reads events from a prophesee .raw file, and packages the events into non-overlapping event windows, each with a
    fixed desired duration as close as possible to the desired one such that resulting windows match a desired
    framerate."""

    def __init__(self, path, n, framerate=1):
        self.raw_reader = RawReader(path)
        self.interval = n * 1000000

        delta_t = int(1 / float(framerate) * 1000000)
        batches = round((delta_t) / self.interval)
        self.interval = (delta_t) if batches == 0 else ((delta_t) / batches)

        if abs(self.interval - n * 1000000) / self.interval > MAX_RELATIVE_ERROR:
            print(
                "WARNING: Can't produce desired framerate with specified interval."
                + " Desired: {} [s], In use: {} [s]".format(n, self.interval / 1000000)
            )

        self.batches = max(1, batches)
        self.counter = 0

    def __iter__(self):
        return self

    def __next__(self):
        if not self.raw_reader.is_done():
            try:
                events = self.raw_reader.load_delta_t(self.interval)
                self.save_flag = self.counter % self.batches == (self.batches - 1)
                self.counter += 1
                return events
            except:
                raise StopIteration

        raise StopIteration

    def at_framerate(self):
        return self.save_flag
