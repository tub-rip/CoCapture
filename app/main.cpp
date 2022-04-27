#include <iostream>

#include <pangolin/var/var.h>
#include <pangolin/var/varextra.h>
#include <pangolin/gl/gl.h>
#include <pangolin/gl/gldraw.h>
#include <pangolin/display/display.h>
#include <pangolin/display/view.h>
#include <pangolin/display/widgets.h>
#include <pangolin/display/default_font.h>
#include <pangolin/handler/handler.h>


void setImageData(unsigned char * imageArray, int size){
    for(int i = 0 ; i < size;i++) {
        imageArray[i] = (unsigned char)(rand()/(RAND_MAX/255.0));
    }
}


int main() {
    pangolin::CreateWindowAndBind("Main",640,480);

    // Panel
    const int UI_WIDTH = 20* pangolin::default_font().MaxWidth();
    pangolin::CreatePanel("ui")
            .SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(UI_WIDTH));
    pangolin::Var<bool> a_button("ui.A_Button",false,false);
    pangolin::Var<double> a_double("ui.A_Double",3,0,5);
    pangolin::Var<int> an_int("ui.An_Int",2,0,5);
    pangolin::Var<double> a_double_log("ui.Log_scale",3,1,1E4, true);
    pangolin::Var<bool> a_checkbox("ui.A_Checkbox",false,true);
    pangolin::Var<int> an_int_no_input("ui.An_Int_No_Input",2);
    pangolin::Var<std::string> a_string("ui.A_String", "Edit ME!");

    // Image
    pangolin::View& d_image = pangolin::Display("image")
            .SetAspect(640.0f/480.0f);

    pangolin::Display("multi")
            .SetBounds(0.0, 1.0, pangolin::Attach::Pix(UI_WIDTH), 1.0)
            .SetLayout(pangolin::LayoutEqual)
            .AddDisplay(d_image);

    const int width =  64;
    const int height = 48;
    unsigned char* imageArray = new unsigned char[3*width*height];
    pangolin::GlTexture imageTexture(width,height,GL_RGB,false,0,GL_RGB,GL_UNSIGNED_BYTE);

    while(!pangolin::ShouldQuit()) {

        // Image
        setImageData(imageArray,3*width*height);
        imageTexture.Upload(imageArray,GL_RGB,GL_UNSIGNED_BYTE);
        d_image.Activate();
        glColor3f(1.0,1.0,1.0);
        imageTexture.RenderToViewport();

        // Necessary
        pangolin::FinishFrame();
    }

    return 0;
}
