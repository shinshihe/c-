#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <math.h>

//read pixel
/*for (int i = 0; i < 500; i++) {
        for (int j = 0; j < 500; j++) {
            cover_rgb[1].at<uchar>(i, j) = 255;
        }
    }*/


// create blank and merge
/*cv::Mat blank, final_image;
blank = cv::Mat::zeros(cv::Size(clone.cols, clone.rows), CV_8UC1);
std::vector<cv::Mat> channels;
channels.push_back(blank);
channels.push_back(cover_rgb[1]);
channels.push_back(cover_rgb[2]);
cv::merge(channels, final_image);
cv::imshow("red", final_image);*/

std::string u_to_s(uchar tmp) {
    std::string result = "";

    uchar left;
    do {
        left = tmp % 2;
        result += std::to_string(left);
        tmp = tmp / 2;
    } while (tmp != 0);

    return result;
}

uchar s_to_u(std::string str) {
    uchar res = 0;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '0') {
            continue;
        }
        else {
            res += pow(2, i);
        }
    }

    return res;

}



void test(cv::Mat img) {
    //cv::imshow("Display window", img);
    for (int i = 0; i < 500; i++) {
        for (int j = 0; j < 500; j++) {
            img.at<uchar>(i, j) = 255;
        }
    }

}

void start_hiding(cv::Mat &cover,int i,int j,std::string str) {
    // i,j is the start point
    str = str + "0000000";

    int count = 0;
    std::string cover_str;
    while (count <= 8) {
        //cover_str = u_to_s(cover.at<uchar>(i, j));
        cover_str = std::to_string(cover.at<uchar>(i, j) % 2);
        if (cover_str[0] > str[count]) {
            cover.at<uchar>(i, j) -= 1;
        }
        else if (cover_str[0] < str[count]) {
            cover.at<uchar>(i, j) += 1;
        }
        count++;
        
        // if need to skip to another line
        if (j == cover.cols - 1) {
            j = 0;
            i += 1;
        }
        else {
            j += 1;
        }

     }
    

}


uchar start_extracting(cv::Mat cover, int i, int j) {
    std::string str = "";
    int count = 0;
    std::string tmp;
    while (count < 8) {
        tmp = u_to_s(cover.at<uchar>(i, j));
        str += tmp[0];
        count++;
        if (j == cover.cols - 1) {
            i += 1;
            j = 0;
        }
        else {
            j += 1;
        }
    }

    uchar res = s_to_u(str);
    return res;
}



int hide_image() {
    // read images
    std::cout << "******** Ok, you are going to hide an image! ********" << std::endl;
    std::cout << "******** Please enter the name of the cover image. ********" << std::endl;
    std::string cover_name;
    std::getline(std::cin, cover_name);

    // std::string cover_name = "cover.png";
    std::string c_image_path = "./" + cover_name;

    std::cout << "******** Please enter the name of the hidden image. ********" << std::endl;
    //std::string hide_name = "hide.png";
    std::string hide_name;
    std::getline(std::cin, hide_name);

    std::string h_image_path = "./" + hide_name;
    cv::Mat origin = cv::imread(c_image_path, cv::IMREAD_COLOR);
    cv::Mat hide = cv::imread(h_image_path, cv::IMREAD_COLOR);

    // if the image can't be read
    if (origin.empty())
    {
        std::cout << "Could not read the image: " << c_image_path << std::endl;
        return 1;
    }
    if (hide.empty())
    {
        std::cout << "Could not read the image: " << h_image_path << std::endl;
        return 1;
    }

    if (hide.rows * hide.cols * 8 > (origin.rows - 1) * origin.cols) {
        std::cout << "the image you want to hide is too big"<<std::endl;
        std::cout << "I have to RESIZE the hidden image"<<std::endl;
        cv::Mat  resize_image;
        float ratio = float(((origin.rows - 1) * origin.cols)) / (hide.rows * hide.cols * 8);
        cv::resize(hide,resize_image,cv::Size(),ratio,ratio);
        hide = resize_image;
    }
    else {
        std::cout << "-------- Good, I can hide it for you! ---------" << std::endl;
        std::cout << "-------- In progress, please wait --------" << std::endl;
    }

    //clone the cover image
    cv::Mat clone = origin.clone();

    // split to three channels
    cv::Mat cover_rgb[3];
    cv::split(clone, cover_rgb);

    cv::Mat hide_rgb[3];
    cv::split(hide, hide_rgb);

    int secret_cols = hide.cols, secret_rows = hide.rows;
    int tmp = cover_rgb[0].at<uchar>(0, 0);
    //cover_rgb
    //store the hidden image's size
    cover_rgb[0].at<uchar>(0, 0) = secret_rows / 255;
    cover_rgb[0].at<uchar>(0, 1) = secret_cols / 255;
    cover_rgb[0].at<uchar>(0, 2) = secret_rows % 255;
    cover_rgb[0].at<uchar>(0, 3) = secret_cols % 255;

    tmp = cover_rgb[0].at<uchar>(0, 0);


    std::string str;
    int pixel_num = 0, pixel_used, start_row, start_col;
    for (int i = 0; i < hide.rows; i++) {
        for (int j = 0; j < hide.cols; j++) {
            //pixel_num =  hide.cols * i + j + 1;
            pixel_num++;
            pixel_used = (pixel_num - 1) * 8;
            start_row = pixel_used / clone.cols + 1;
            start_col = pixel_used % clone.cols;
            // each pixel of the hidden image should be transfer to binary strings
            for (int k = 0; k < 3; k++) {
                //hide?cover rgb
                str = u_to_s(hide_rgb[k].at<uchar>(i, j));
                start_hiding(cover_rgb[k], start_row,start_col, str);
            }
            
        }
        if (i % 100 == 0) { std::cout << i << "/" << hide.rows << std::endl; }
     }

    
    std::vector<cv::Mat> channels;
    cv::Mat final_image;
    tmp = cover_rgb[0].at<uchar>(0, 0);
    channels.push_back(cover_rgb[0]);
    channels.push_back(cover_rgb[1]);
    channels.push_back(cover_rgb[2]);
    cv::merge(channels, final_image);

    std::string file_name = "./secret_" + cover_name;
    cv::imwrite(file_name, final_image);
    std::cout << "hide complete" << std::endl;
    std::cout << "the file name is " << "secret_" << cover_name <<std::endl;




    
    return 0;
}



int extract_image() {
    std::cout << "******** Ok, you are now trying to extract a secret image from the cover. ********" << std::endl;
    std::cout << "!!!! Warning, if the cover does not contains a hidden image, the program may crash !!!!" << std::endl;
    std::cout << "-------- Please enter the image name --------" << std::endl;
    //std::string cover_name = "secret_cover.png";
    std::string cover_name;
    std::getline(std::cin, cover_name);
    
    std::string path = "./" + cover_name;
    cv::Mat origin = cv::imread(path, cv::IMREAD_COLOR);

    if (origin.empty())
    {
        std::cout << "Could not read the image: " << path << std::endl;
        return 1;
    }

    /*cover_rgb[0].at<uchar>(0, 0) = secret_rows / 255;
    cover_rgb[0].at<uchar>(0, 1) = secret_cols / 255;
    cover_rgb[0].at<uchar>(0, 2) = secret_rows % 255;
    cover_rgb[0].at<uchar>(0, 3) = secret_cols % 255;*/
    cv::Mat cover_rgb[3];
    cv::split(origin, cover_rgb);

    //int tmp = cover_rgb[0].at<uchar>(0, 0);
    //int tmp1 = cover_rgb[0].at<uchar>(0, 1);
    //int tmp2 = cover_rgb[0].at<uchar>(0, 2);

    int hide_row = cover_rgb[0].at<uchar>(0, 0) * 255 + cover_rgb[0].at<uchar>(0, 2);
    int hide_col = cover_rgb[0].at<uchar>(0, 1) * 255 + cover_rgb[0].at<uchar>(0, 3);

    // create empty 3 channel image;
    cv::Mat final_image;
    cv::Mat hide(hide_row, hide_col, CV_8UC3);

    cv::Mat hide_rgb[3];
    cv::split(hide, hide_rgb);
    
    std::cout << "-------- Extracting, please wait for a moment --------" << std::endl;
    int pixel_num = 0, pixel_used, start_row, start_col;
    uchar value;
    for (int i = 0; i < hide_row; i++) {
        for (int j = 0; j < hide_col; j++) {
            //pixel_num = hide_col * i + j + 1;
            pixel_num++;
            pixel_used = (pixel_num - 1) * 8;
            start_row = pixel_used / origin.cols + 1;
            start_col = pixel_used % origin.cols;
            for (int k = 0; k < 3; k++) {
                // now it's time to extract!
                value = start_extracting(cover_rgb[k], start_row, start_col);
                hide_rgb[k].at<uchar>(i, j) = value;
                //std::cout << int(value) << " " << std::endl;
            }
        }
        if (i % 100 == 0) { std::cout << i << "/" << hide_row << std::endl; }
        
    }
    //std::cout << "pause here" << std::endl;

    std::vector<cv::Mat> channels;

    channels.push_back(hide_rgb[0]);
    channels.push_back(hide_rgb[1]);
    channels.push_back(hide_rgb[2]);
    cv::merge(channels, final_image);
    // cv::imshow("tmp", final_image);
    cv::imwrite("extracted_secret.png", final_image);
    std::cout << "extract complete" << std::endl;

    return 0;
    //std::cout << "pause here" << std::endl;

    

}


int main()
{   
    std::cout << "---------- Welcome to John's image hide program ----------" << std::endl;
    std::cout << "---------- I know you must be an interesting person ( LSP/Shinshi )" << std::endl;
    std::cout << "---------- Just a kind reminder, plz select a big image to be the cover --------" << std::endl;
    std::cout << "---------- The cover should be 8 times bigger than the hidden image --------" << std::endl;
    std::cout << "---------- And please make sure these two images are not jpg files --------" << std::endl;
    std::cout << "---------- Okay, time to start -------- " << std::endl << "Enter 1 to hide image, enter 2 to extract image" << std::endl;

    int command;
    std::cin >> command;
    std::cin.ignore();
    if (command == 1) {
        int hide = hide_image();
    }
    else if (command == 2) {
        int extract = extract_image();
    }
    else {
        std::cout << "******** come on, give me 1 or 2 ********" << std::endl;
        std::cout << "******** I'm angry, kuola!!! progrma ENDs XD ********" << std::endl;
    }

    

    /*
    cv::Mat img = cv::imread("./cover.png");
    cv::Mat img2;
    cv::resize(img,img2,cv::Size(),0.5,0.5);
    cv::imwrite("cover_50.png", img2);*/

    //cv::imshow("Display window", cover_rgb[1]);
    //test(cover_rgb[1]);
    //::imshow("tmp", cover_rgb[1]);

    /*cv::Mat img = cv::imread("./test.jpg", cv::IMREAD_COLOR);
    std::vector<cv::Mat> channels;
    cv::Mat cover_rgb[3],final_image;
    cv::split(img, cover_rgb);

    channels.push_back(cover_rgb[0]);
    channels.push_back(cover_rgb[1]);
    channels.push_back(cover_rgb[2]);
    cv::merge(channels, final_image);
    cv::imwrite("test2.jpg", final_image);*/


    std::cout << "the program ends" << std::endl;

    

    
    
    //int k = cv::waitKey(0); // Wait for a keystroke in the window
    /*if (k == 's')
    {
        //cv::imwrite("starry_night.png", clone);
    }*/
    return 0;
}
