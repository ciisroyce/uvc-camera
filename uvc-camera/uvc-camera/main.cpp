//
//  main.cpp
//  uvc-camera
//
//  Created by Royce Slick on 7/8/21.
//

//#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////

#include "libuvc/libuvc.h"
#include <stdio.h>
#include <unistd.h>
#include "MyImage.h"
#include "BmpInfo.h"
#include "IImageBuffer.h"
#include <iostream>

//#include <opencv2/opencv.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/core.hpp>
//#include <opencv2/core/core_c.h>
//#include <opencv2/core/types_c.h>
//#include <opencv2/highgui/highgui_c.h>

/* This callback function runs once per frame. Use it to perform any
 * quick processing you need, or have it put the frame into your application's
 * input queue. If this function takes too long, you'll start losing frames. */
void cb(uvc_frame_t *frame, void *ptr) {
    uvc_frame_t *bgr;
    uvc_error_t ret;
    enum uvc_frame_format *frame_format = (enum uvc_frame_format *)ptr;
    /* FILE *fp;
     * static int jpeg_count = 0;
     * static const char *H264_FILE = "iOSDevLog.h264";
     * static const char *MJPEG_FILE = ".jpeg";
     * char filename[16]; */
    
    /* We'll convert the image from YUV/JPEG to BGR, so allocate space */
    bgr = uvc_allocate_frame(frame->width * frame->height * 3);
    if (!bgr) {
        printf("unable to allocate bgr frame!\n");
        return;
    }
    
    printf("\nEntering callback...\n frame_format = %d, width = %d, height = %d, length = %zu, ptr = %zu\n",
           frame->frame_format, frame->width, frame->height, frame->data_bytes, (size_t)ptr); // RS
    
    switch (frame->frame_format) {
            /*
             case UVC_FRAME_FORMAT_H264:
             printf ("UVC_FRAME_FORMAT_H264\n");
             // use `ffplay H264_FILE` to play
             // fp = fopen(H264_FILE, "a");
             // * fwrite(frame->data, 1, frame->data_bytes, fp);
             // * fclose(fp);
             break;
             */
        case UVC_COLOR_FORMAT_MJPEG:
            printf ("UVC_COLOR_FORMAT_MJPEG\n");
            /* sprintf(filename, "%d%s", jpeg_count++, MJPEG_FILE);
             * fp = fopen(filename, "w");
             * fwrite(frame->data, 1, frame->data_bytes, fp);
             * fclose(fp); */
            break;
        case UVC_COLOR_FORMAT_YUYV:
            //printf ("UVC_COLOR_FORMAT_YUYV\n");    /* Do the BGR conversion */
            ret = uvc_any2bgr(frame, bgr);
            if (ret) {
                uvc_perror(ret, "uvc_any2bgr");
                uvc_free_frame(bgr);
                return;
            }
            else {
                //imshow("Display window", frame);
                //imwrite("captured_image.png", frame);
            }
            break;
        default:
            break;
    }
    
    if (frame->sequence % 30 == 0) {
        printf(" * got image %u\n",  frame->sequence);
    }
    
    // Use Brad's function to save a single frame
    MyImage im(640, 480);
    BYTE *pData = im.grabWrite();
    
    // Process one frame
    memset(pData, 0, im.getInfo().size.h + im.getInfo().size.w + im.getInfo().BPP);
    std::cout << "Buffer size = " + std::to_string(im.getInfo().size.h * im.getInfo().size.w * im.getInfo().BPP) << std::endl;
    std::cout << "frame->data_bytes = " + std::to_string(frame->data_bytes) << " This is a YUV buffer" << std::endl;
    std::cout << "bgr->data_bytes = " + std::to_string(bgr->data_bytes)  << " This is an RGB buffer" << std::endl;
    std::cout << "h,w,bpp = " + std::to_string(im.getInfo().size.h) << ", " << std::to_string(im.getInfo().size.w) << ", " << std::to_string(im.getInfo().BPP) << std::endl;
    
    for (int row = 0 ; row < im.getInfo().size.h ; row++) {     // rows from bottom
        int destRow = im.getInfo().size.h - 1 - row;            // Opposite direction for flipping the image
        for (int col = 0 ; col < im.getInfo().size.w ; col++) { // columns from left
            for (int rgb = 0; rgb < im.getInfo().BPP; rgb++) {  // R,G,B
                //pData[j*im.getInfo().stride + i * im.getInfo().BPP + c] = myImagebyte;
                pData[destRow * im.getInfo().stride + col * im.getInfo().BPP + rgb] = *((char *)bgr->data
                                                                                        + (row * im.getInfo().size.w * im.getInfo().BPP) // Offset to the row
                                                                                        + (col * im.getInfo().BPP)                       // Offset to the column
                                                                                        + rgb);                                          // Offset to the byte
            }
        }
    }
    
    im.release(pData);
    std::string filename = "frame_" + std::to_string(frame->sequence) + ".bmp";
    im.Save(filename.c_str());
    
    /* Call a user function:
     *
     * my_type *my_obj = (*my_type) ptr;
     * my_user_function(ptr, bgr);
     * my_other_function(ptr, bgr->data, bgr->width, bgr->height);
     */
    
    /* Call a C++ method:
     *
     * my_type *my_obj = (*my_type) ptr;
     * my_obj->my_func(bgr);
     */
    
    /* Use opencv.highgui to display the image:
     *
     * cvImg = cvCreateImageHeader(
     *     cvSize(bgr->width, bgr->height),
     *     IPL_DEPTH_8U,
     *     3);
     *
     * cvSetData(cvImg, bgr->data, bgr->width * 3);
     *
     * cvNamedWindow("Test", CV_WINDOW_AUTOSIZE);
     * cvShowImage("Test", cvImg);
     * cvWaitKey(10);
     *
     * cvReleaseImageHeader(&cvImg);
     */
    
    uvc_free_frame(bgr);
}

void dumpDeviceDescriptor(uvc_device_t *dev, uvc_device_descriptor_t *desc) {
    uvc_get_device_descriptor(dev, &desc);
    std::cout << "The following items were included in the device description: " << std::endl;
    // Product
    std::cout << "   Product: " << desc->product << std::endl;
    
    //Product ID
    std::cout << "   Product ID: " << desc->idProduct << std::endl;
    
    // Vendor ID
    std::cout << "   Vendor ID: " << desc->idVendor ; // no endl
    if (desc->idVendor == 1133) {
        std::cout << " (Logitech)" << std::endl;
    }
    else {
        std::cout << std::endl;
    }
    
    //Manufacturer
    if (desc->manufacturer) {
        std::cout << "   Manufacturer: " << desc->manufacturer << std::endl;
    }
    else {
        std::cout << "   Manufacturer: null" << std::endl;
    }
    
    // Serial Number
    std::cout << "   Serial Number: " << desc->serialNumber << std::endl;
}

int main(int argc, char **argv) {
    uvc_context_t *ctx;
    uvc_device_t *dev;
    uvc_device_handle_t *devh;
    uvc_stream_ctrl_t ctrl;
    uvc_error_t res;
    uvc_device_descriptor_t *desc = nullptr;
    int busNum = 0;
    uvc_device_t **list = nullptr;

    /* Initialize a UVC service context. Libuvc will set up its own libusb
     * context. Replace NULL with a libusb_context pointer to run libuvc
     * from an existing libusb context. */
    res = uvc_init(&ctx, NULL);
    
    if (res < 0) {
        uvc_perror(res, "uvc_init");
        return res;
    }
    
    puts("UVC initialized");
    
    // TODO: Running this list code stops the camera from properly starting streaming
//    res = uvc_get_device_list(ctx, &list);
//    std::cout << "Accessing the the list ..." << std::endl; // TODO: How do we know how many items are in the list???
//    for(int i = 0; i < 1; i++) {
//        res = uvc_open(list[i], &devh);
//        uvc_close(devh);
//    }
//    unsigned char x = 0;
//    uvc_free_device_list(list, x);
    
    /* Locates the first attached UVC device, stores in dev */
    std::cout << "Locating the first attached UVC device..." << std::endl;
    res = uvc_find_device(ctx, &dev, 0, 0, NULL); // filter devices by vendor_id, product_id, "serial_num"
        if (res < 0) {
        uvc_perror(res, "uvc_find_device");
            uvc_exit(ctx);
            std::cout << "Exiting..." << std::endl;
            exit(1);
    } else {
        puts("Device found");
        
        // Find the bus number to which the device is connected
        busNum = uvc_get_bus_number (dev);
        std::cout << "The device is connected to bus number: " << busNum << std::endl;

        // Print out the contents of the device descriptory
        dumpDeviceDescriptor(dev, desc);

        /* Try to open the device: requires exclusive access */
        std::cout << "Opening the device..." << std::endl;
        
        res = uvc_open(dev, &devh);
        if (res < 0) {
            uvc_perror(res, "uvc_open");
            uvc_exit(ctx);
            std::cout << "Exiting..." << std::endl;
            exit(1);
        } else {
            puts("Device opened");
            
            // Print out all of the information that libuvc knows about the device
            //std::cout << std::endl << "The following dump is everything libuvc knows about the device:" << std::endl;
            //uvc_print_diag(devh, stderr);
            
            const uvc_format_desc_t *format_desc = uvc_get_format_descs(devh);
            const uvc_frame_desc_t *frame_desc = format_desc->frame_descs;
            enum uvc_frame_format frame_format;
            int width = 640;
            int height = 480;
            int fps = 30;
            
            switch (format_desc->bDescriptorSubtype) {
                case UVC_VS_FORMAT_MJPEG:
                    frame_format = UVC_COLOR_FORMAT_MJPEG;
                    break;
                    //case UVC_VS_FORMAT_FRAME_BASED:
                    //  frame_format = UVC_FRAME_FORMAT_H264;
                    //  break;
                default:
                    frame_format = UVC_FRAME_FORMAT_YUYV;
                    break;
            }
            
            if (frame_desc) {
                width = frame_desc->wWidth;
                height = frame_desc->wHeight;
                fps = 10000000 / frame_desc->dwDefaultFrameInterval;
            }
            
            printf("\nFirst format: (%4s) %dx%d %dfps\n", format_desc->fourccFormat, width, height, fps);
            
            /* Try to negotiate first stream profile */
            res = uvc_get_stream_ctrl_format_size(
                                                  devh, &ctrl, /* result stored in ctrl */
                                                  frame_format,
                                                  width, height, fps /* width, height, fps */
                                                  );
            
            /* Print out the result */
            uvc_print_stream_ctrl(&ctrl, stderr);
            
            if (res < 0) {
                uvc_perror(res, "get_mode"); /* device doesn't provide a matching stream */
            } else {
                /* Start the video stream. The library will call user function cb:
                 *   cb(frame, (void *) 12345)
                 */
                res = uvc_start_streaming(devh, &ctrl, cb, (void *) 12345, 0); // TODO: This generates: "attempt to claim already-claimed interface 1"
                
                if (res < 0) {
                    uvc_perror(res, "start_streaming"); /* unable to start stream */
                } else {
                    puts("Streaming...");
                    
                    /* enable auto exposure - see uvc_set_ae_mode documentation */
                    puts("Enabling auto exposure ...");
                    const uint8_t UVC_AUTO_EXPOSURE_MODE_AUTO = 2;
                    res = uvc_set_ae_mode(devh, UVC_AUTO_EXPOSURE_MODE_AUTO);
                    if (res == UVC_SUCCESS) {
                        puts(" ... enabled auto exposure");
                    } else if (res == UVC_ERROR_PIPE) {
                        /* this error indicates that the camera does not support the full AE mode;
                         * try again, using aperture priority mode (fixed aperture, variable exposure time) */
                        puts(" ... full AE not supported, trying aperture priority mode");
                        const uint8_t UVC_AUTO_EXPOSURE_MODE_APERTURE_PRIORITY = 8;
                        res = uvc_set_ae_mode(devh, UVC_AUTO_EXPOSURE_MODE_APERTURE_PRIORITY);
                        if (res < 0) {
                            uvc_perror(res, " ... uvc_set_ae_mode failed to enable aperture priority mode");
                        } else {
                            puts(" ... enabled aperture priority auto exposure mode");
                        }
                    } else {
                        uvc_perror(res, " ... uvc_set_ae_mode failed to enable auto exposure mode");
                    }
                    
                    sleep(1); /* stream for 1 second */
                    
                    /* End the stream. Blocks until last callback is serviced */
                    uvc_stop_streaming(devh);
                    puts("Done streaming.");
                }
            }
            
            /* Release our handle on the device */
            uvc_close(devh);
            puts("Device closed");
        }
        
        /* Release the device descriptor */
        uvc_unref_device(dev);
    }
    
    /* Close the UVC context. This closes and cleans up any existing device handles,
     * and it closes the libusb context if one was not provided. */
    uvc_exit(ctx);
    puts("UVC exited");
    
    return 0;
}

