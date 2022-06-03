#define pipeline
// �p�C�v���C���ƒ����������r����ׂ̏�������
#ifdef pipeline

#include "MyOpenCV.h"
#include <chrono>
#include <iomanip>
#include <vector>



// caffe��300*300�摜����͂Ƃ��ē���
#define inWidth 300
#define inHeight 300

// Blur�p
#define CV_GAUSSIAN 0
#define  CV_BLUR 1
#define CV_MEDIAN 2
#define NONE 3

// �猟�o�p
#define inScaleFacter  (double)1.0
#define confidenceThreshold (float)0.5

// ���[�v�u���C�N�p
#define STOP 1

// �C�x���g�I�u�W�F�N�g��
#define EVENT_NAME1  L"EVENT1"
#define EVENT_NAME2  L"EVENT2"
#define EVENT_NAME3  L"EVENT3"
#define EVENT_NAME4  L"EVENT4"

// ��i�X���b�h�𓯊�������(���񃋁[�v)
#define START1  L"START1"
#define START2  L"START2"
#define START3  L"START3"
#define START4  L"START4"

using namespace std;

// ���Ԍv���p(�v���O�����N��������̎��Ԃ��v��)
chrono::system_clock::time_point keisoku1;
chrono::system_clock::time_point keisoku2;
chrono::system_clock::time_point keisoku3;
chrono::system_clock::time_point keisoku4;
chrono::system_clock::time_point keisoku5;
chrono::system_clock::time_point keisoku6;
chrono::system_clock::time_point keisoku7;
chrono::system_clock::time_point keisoku8;
chrono::system_clock::time_point keisoku9;
chrono::system_clock::time_point keisoku10;

// frame�N���X
template <class T>
class FRAME{
    
public:
    cv::VideoCapture cap;
    cv::Mat img;
    cv::Mat img2;
    cv::Mat img3;
    cv::Mat img4;
    // ���摜�̏c��
    int frameWidth;
    int frameHeight;
    /*cv::dnn::Net net;*/
    // �t���[������
    int slide = 0;
    // �������[�v�I��
    int Key = 0;
#ifdef _DEBUG
    // video path
    string camID = "D:/ss/ss07/ss07/sample.MP4";
#else
    // camera ID
    int camID = 0;

#endif
    // �u���[�I�v�V����
    int BlurMode = CV_GAUSSIAN;
    // �u���[���x
    int sigma = 0;

    // rect_change
    bool rectChange = true;

    // net
    cv::dnn::Net net;

    // 1�t���[���Ɍ��o������(��2�p�̍��W)�����ׂĊi�[���铮�I�Ȕz��(vector)
    vector<T> detectedFacesX1copy;
    vector<T> detectedFacesY1copy;
    vector<T> detectedFacesX2copy;
    vector<T> detectedFacesY2copy;

    //T faceCoordinates; // ���4�p�̍��W
    // ���������s���֐��@ �~�`�Ƀu���[���邩�C�猟�o�����͈͂����}�X�N�擾���ău���[�����邩
    void cvSmooth(cv::Mat& src, cv::Mat& dst, int mode);
    // �t���X�N���[���\���̊֐�
    int ShowImageFullScreen(cv::Mat& src);

    // ����t�@�C���������o�����߂̃I�u�W�F�N�g��錾����
    cv::VideoWriter writer;
    int    width, height; // �쐬���铮��t�@�C���̐ݒ�
    int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v'); // �r�f�I�t�H�[�}�b�g�̎w��( ISO MPEG-4 / .mp4)
    float fps;
    // �ۑ�����t�@�C����
    string filename;

    // ���Ԍv���p(�v���O�����N��������̎��Ԃ��v��)
    chrono::system_clock::time_point start;
};

//---------------------------------------------------------------------------------

DWORD WINAPI CapCamera(LPVOID lpparameter) {
    // �f�o�b�O���[�h�ŃT���v������̓ǂݍ��݁C�����[�X���[�h��camera����̓ǂݍ���
    // �^�悵�������ǂݍ���
    // ��i�X���b�h�̊J�n�C�x���g
    // �C�x���g�I�u�W�F�N�g���擾���܂�
   /*�C�x���g�I�u�W�F�N�g���I�[�v������D
   �������F�I�[�v������C�x���g�I�u�W�F�N�g�ɑ΂��Ăǂ̂悤�ȃA�N�Z�X���s�����w�肷��t���O
   �������F�擾�����n���h�����p���\���ۂ���\��
   ��O�����F�I�[�v������C�x���g�I�u�W�F�N�g�̖��O
   �߂�l�F�I�[�v�������C�x���g�̖߂�l(���s��NULL)*/
    HANDLE start1 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START1);
    HANDLE hEvent1 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME1);
   // �|�C���^�ł��邱�Ƃ𖾎�����
    FRAME<int>* pt = (FRAME<int>*)lpparameter;
#ifdef _DEBUG
    pt->cap.open(pt->camID);
    // �v���O�����J�n����̎��Ԃ��v��
    pt->start = std::chrono::system_clock::now();
    if (pt->cap.isOpened() == false) {
    // ����t�@�C�����J���Ȃ������Ƃ��͏I������
        cerr << "can't load video" << endl;
        return -1;
    }
#else /*RELEASE*/
    
    pt->cap.open(pt->camID); // �f�t�H���g�J�������I�[�v��
    pt->start = std::chrono::system_clock::now();
    if (!pt->cap.isOpened()) {  // �����������ǂ������`�F�b�N
        cerr << "can't open video!" << endl;
        return -1;
    }
#endif /*_DEBUG OR RELEASE*/
    while (true) {
        keisoku1 = std::chrono::system_clock::now();
        pt->cap >> pt->img; //USB�J��������������̂P�t���[�����i�[    
        
        // ���摜�̏c���̃T�C�Y(�A�m�e�[�V�������ʂ��������΂��Ƃ��̃T�C�Y�v�Z�p)
        pt->frameWidth = pt->img.cols;
        pt->frameHeight = pt->img.rows;

        // ����ۑ��Ɖ�ʕ\���̍ۂ̏����擾
        pt->width = (int)pt->cap.get(cv::CAP_PROP_FRAME_WIDTH);	// �t���[���������擾
        pt->height = (int)pt->cap.get(cv::CAP_PROP_FRAME_HEIGHT);	// �t���[���c�����擾
        pt->fps = (float)pt->cap.get(cv::CAP_PROP_FPS);				// �t���[�����[�g���擾
        
        // �C�x���g�I�u�W�F�N�g���V�O�i����ԂɂȂ�܂ő҂��܂�
        WaitForSingleObject(hEvent1, INFINITE); 
        // ���̃X���b�h�ɓn���ϐ�
        pt->img2 = pt->img.clone(); // �R�s�[��n��
        // ���ڂ̃J�������͂�����΁C��i�X���b�h�̃��[�v���J�n������
        SetEvent(start1); // START1�C�x���g(�猟�o�X���b�h)���V�O�i����Ԃɂ���
        keisoku2 = std::chrono::system_clock::now();
        stringstream time1;
        time1 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku2 - keisoku1).count()) / 1000; // �����_�Ƃ���������
        cout << "time1: " << time1.str() << endl;
        if (pt->Key == 1) {
            break;
        }
    }
    // �X���b�h�̏I��
    return 0; // 0��Ԃ�
}

DWORD WINAPI DetectFace(LPVOID lpparameter) {
    
    // �|�C���^�ł��邱�Ƃ𖾎�����
    FRAME<int>* pt = (FRAME<int>*)lpparameter;
    
    //------------------------------------------------------------------------------------------------------------------
    // �猟�o�X���b�h�̊J�n�C�x���g
    HANDLE start1 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START1);
    // �ڂ����X���b�h�̊J�n�C�x���g
    HANDLE start2 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START2);
    
    // �C�x���g�I�u�W�F�N�g�̎擾 
    HANDLE hEvent2 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME2);
    // �C�x���g�I�u�W�F�N�g���擾
    HANDLE hEvent1 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME1);
    // �C�x���g�I�u�W�F�N�g���V�O�i����ԂɂȂ�܂ő҂��܂��B
   
    // �f�o�b�O���[�h�ŃT���v������̓ǂݍ��݁C�����[�X���[�h��camera����̓ǂݍ���
    // �C���[�W��Blob�ɕϊ�����Aforward() �֐����g�p���ăl�b�g���[�N��ʉ� �O����
    // 1.���ό��Z�ƃX�P�[�����O�𒲂ׂ�(�I�v�V�����Ń`�����l���X���b�s���O)
   
    while (true) {
        keisoku3 = std::chrono::system_clock::now();
        WaitForSingleObject(start1, INFINITE); // img2�ɉ摜�����͂����܂ő҂�
        //----------------�O����-----------------------------------------------------------------------------------------------
        cv::Mat inputBlob = cv::dnn::blobFromImage(pt->img2, 0.5, cv::Size(inWidth, inHeight), false, false);
        pt->net.setInput(inputBlob); // net��blob��n��
        cv::Mat detection = pt->net.forward(); //4-dim:nchw
        // N: Number of detected objects as face
        // C: color channels
        // H: Height
        // W: Width
    //----------------------------------------------------------------��������猟�m--------------------------------------------------------------------
        cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
        // detection.size[2]: �m�M�x
        // detection.size[3]: x1, y1, x2, y2�E�E�E
        // �t���[�����ƂɐV����vector��ݒ肷��
        vector<int> detectedFacesX1;
        vector<int> detectedFacesY1;
        vector<int> detectedFacesX2;
        vector<int> detectedFacesY2;
        int sizeChangerX1 = 0;
        int sizeChangerY1 = 0;
        int sizeChangerX2 = 0;
        int sizeChangerY2 = 0;
        int j = 0;
        for (int i = 0; i < detectionMat.rows; i++)
        {
            float confidence = detectionMat.at<float>(i, 2); // ���o�������ʂ̐M���x�̉����l
            // ��ƌ��o����
            if (confidence > confidenceThreshold)
            {
                detectedFacesX1.push_back(static_cast<int>(detectionMat.at<float>(i, 3) * pt->frameWidth));
                detectedFacesY1.push_back(static_cast<int>(detectionMat.at<float>(i, 4) * pt->frameHeight));
                detectedFacesX2.push_back(static_cast<int>(detectionMat.at<float>(i, 5) * pt->frameWidth));
                detectedFacesY2.push_back(static_cast<int>(detectionMat.at<float>(i, 6) * pt->frameHeight));
                // img�ɐԘg��������(�Ԙg�\���E��\����؂�ւ�����悤�ɂ���) �R�}���h���C���p�[�T�[�ł�����
                // �t���[���J�E���g�C�^�C���X�^���v�̕\���E��\���؂�ւ�
//-----------------------------------------���o������ɐԘg��������C���o����\��(�؂�ւ���悤�ɂ���)--------------------------------------------------------
                if (GetAsyncKeyState(VK_DELETE)) {
                    pt->rectChange = false;
                }
                if (GetAsyncKeyState(VK_INSERT)) {
                    pt->rectChange = true;
                }
                if(pt->rectChange == true) {
                    cv::rectangle(pt->img2, cv::Point(detectedFacesX1[j] - sizeChangerX1, detectedFacesY1[j] - sizeChangerY1), cv::Point(detectedFacesX2[j] - sizeChangerX2, detectedFacesY2[j] - sizeChangerY2), cv::Scalar(0, 0, 255), 2, 4);
                }
                
                stringstream ss;
                ss << fixed << setprecision(2) << confidence * 100;

                cv::putText(
                    pt->img2,
                    ss.str() + "%",
                    cv::Point(detectedFacesX1[j], detectedFacesY1[j]),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.5,
                    cv::Scalar(0, 0, 255),
                    1
                );
                j++;
            }
        }

        //-----------------------------------------------�����܂Ŋ猟�o-----------------------------------------------------------------------------------------------
        // �C�x���g�I�u�W�F�N�g���V�O�i����ԂɂȂ�܂ő҂��܂��B
        WaitForSingleObject(hEvent2, INFINITE); // �ڂ����X���b�h��҂�
        pt->img3 = pt->img2.clone(); // �R�s�[��n��
        // �J�������̓X���b�h�𓯊�������
        SetEvent(hEvent1);
        // vector�̃R�s�[��n��
        std::copy(detectedFacesX1.begin(), detectedFacesX1.end(), std::back_inserter(pt->detectedFacesX1copy));
        std::copy(detectedFacesY1.begin(), detectedFacesY1.end(), std::back_inserter(pt->detectedFacesY1copy));
        std::copy(detectedFacesX2.begin(), detectedFacesX2.end(), std::back_inserter(pt->detectedFacesX2copy));
        std::copy(detectedFacesY2.begin(), detectedFacesY2.end(), std::back_inserter(pt->detectedFacesY2copy));
        // Blur�X���b�h�ɓn�����������i�[���I�����̂ŁC(���񃋁[�v)�X�^�[�g�C�x���g���V�O�i���ɂ���
        SetEvent(start2); // start2���V�O�i����Ԃɂ���
        keisoku4 = std::chrono::system_clock::now();
        stringstream time2;
        time2 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku4 - keisoku3).count()) / 1000; // �����_�Ƃ���������
        cout << "time2: " << time2.str() << endl;
        if (pt->Key == 1) {
            break;
        }
    } 
    // �X���b�h�̏I��
    return 0; // 0��Ԃ�
}

DWORD WINAPI Blur(LPVOID lpparameter) {
    // �|�C���^�ł��邱�Ƃ𖾎�����
    FRAME<int>* pt = (FRAME<int>*)lpparameter;
    // �ڂ����X���b�h�̊J�n�C�x���g
    HANDLE start2 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START2);
    // �\���E�ۑ��X���b�h�̊J�n�C�x���g
    HANDLE start3 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START3);
    // �\���E�ۑ��X���b�h�̊J�n�C�x���g
    HANDLE start4 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START4);


    // �C�x���g�I�u�W�F�N�g�̎擾 // �\���X���b�h��҂�
    HANDLE hEvent3 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME3);
    // �C�x���g�I�u�W�F�N�g�̎擾 // �ۑ��X���b�h��҂�
    HANDLE hEvent4 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME4);

    // �C�x���g�I�u�W�F�N�g�̎擾 // ���o�X���b�h���V�O�i����������
    HANDLE hEvent2 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME2);
   
    while (true) {
        keisoku5 = std::chrono::system_clock::now();
        // �C�x���g�I�u�W�F�N�g���V�O�i����ԂɂȂ�܂ő҂��܂�
        WaitForSingleObject(start2, INFINITE);

        // �f�o�b�O���[�h�ŃT���v������̓ǂݍ��݁C�����[�X���[�h��camera����̓ǂݍ���
        int j = 0;
        for (auto& entry : pt->detectedFacesX1copy) {
            cv::Mat roi = cv::Mat(pt->img3, cv::Rect(pt->detectedFacesX1copy[j], pt->detectedFacesY1copy[j], pt->detectedFacesX2copy[j] - pt->detectedFacesX1copy[j], pt->detectedFacesY2copy[j] - pt->detectedFacesY1copy[j]));
            //-------------------------------------------------------�u���[��������------------------------------------------------------------------------------------------
            pt->cvSmooth(roi, roi, pt->BlurMode); // �K�E�V�A���u���[�ŕ��������s���� ��O�����Ńu���[��؂�ւ�����
            
            j++;
        }
        // �\���ƕۑ���҂�
        WaitForSingleObject(hEvent3, INFINITE);
        WaitForSingleObject(hEvent4, INFINITE);
        pt->img4 = pt->img3.clone();
       
        SetEvent(hEvent2); // detect���V�O�i����Ԃɂ���
        // �s�v�ȃ��������J��
        pt->detectedFacesX1copy.clear();
        pt->detectedFacesY1copy.clear();
        pt->detectedFacesX2copy.clear();
        pt->detectedFacesY2copy.clear();
        SetEvent(start3); // �\���X���b�h���V�O�i����������
        SetEvent(start4); // �ۑ��X���b�h���V�O�i����������
        keisoku6 = std::chrono::system_clock::now();
        stringstream time3;
        time3 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku6 - keisoku5).count()) / 1000; // �����_�Ƃ���������
        cout << "time3: " << time3.str() << endl;
        if (pt->Key == 1) {
            break;
        }
    }
    // �X���b�h�̏I��
    return 0; // 0��Ԃ�
}

DWORD WINAPI ShowScreen(LPVOID lpparameter) {
    // �|�C���^�ł��邱�Ƃ𖾎�����
    FRAME<int>* pt = (FRAME<int>*)lpparameter;
    // �\���E�ۑ��X���b�h�̊J�n�C�x���g
    HANDLE start3 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START3);

    // �C�x���g�I�u�W�F�N�g�̎擾 // �\���E�ۑ��X���b�h��҂�
    HANDLE hEvent3 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME3);
    
    while (true) {
        keisoku7 = std::chrono::system_clock::now();
        // �C�x���g�I�u�W�F�N�g���V�O�i����ԂɂȂ�܂ő҂��܂��B
        WaitForSingleObject(start3, INFINITE);
        // �f�o�b�O���[�h�ŃT���v������̓ǂݍ��݁C�����[�X���[�h��camera����̓ǂݍ���
        // �t���[�����̕\��
        pt->slide += 1;
        // BaskSpace�L�[����������C���ԂȂǂ�\�����Ȃ�
        // �v���O�����N��������̎��Ԃ�\��
        auto end = std::chrono::system_clock::now();  // �v���O�����N������̌v���I������
        auto diff = end - pt->start; // �o�ߎ���
        stringstream time;
        time << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(diff).count()) / 1000; // �����_�Ƃ���������
        if (GetAsyncKeyState(VK_BACK)) {
        }
        else {
            cv::putText(
                pt->img4,
                "Frame:" + to_string(pt->slide),
                cv::Point(6, 15),
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0, 0, 255),
                1
            );
            cv::putText(
                pt->img4,
                "time:" + time.str() + " [s]",
                cv::Point(100, 15),
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0, 0, 255),
                1
            );
        }
        if (!GetAsyncKeyState(VK_RBUTTON)) {
            cv::putText(
                pt->img4,
                "help: click left mouse!",
                cv::Point(6, 470),
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0, 0, 255),
                1
            );
        }
        
        // �}�E�X�̍��{�^���Ńw���v�\�� 
        if (GetAsyncKeyState(VK_RBUTTON)) {
            cv::putText(
                pt->img4,
                "Q:quit F1:NormalBlur F2:GaussianBlur(up, down) F3:MedianBlur F4:NONE",
                cv::Point(6, 450),
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0, 0, 255),
                1
            );
            cv::putText(
                pt->img4,
                "Delete:delete annotation Insert:display annotation backspace:delete time",
                cv::Point(6, 470),
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0, 0, 255),
                1
            );
        }
        pt->ShowImageFullScreen(pt->img4);
        SetEvent(hEvent3); // hEvent3���V�O�i����Ԃɂ���
        keisoku8 = std::chrono::system_clock::now();
        stringstream time4;
        time4 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku8 - keisoku7).count()) / 1000; // �����_�Ƃ���������
        cout << "time4: " << time4.str() << endl;
        if (pt->Key == 1) {
            break;
        }
    }
    // �X���b�h�̏I��
    return 0; // 0��Ԃ�
}

DWORD WINAPI SaveFile(LPVOID lpparameter) {

    // �\���E�ۑ��X���b�h�̊J�n�C�x���g
    HANDLE start4 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START4);
    // �C�x���g�I�u�W�F�N�g�̎擾 // �\���E�ۑ��X���b�h��҂�
    HANDLE hEvent4 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME4);
   
    // �|�C���^�ł��邱�Ƃ𖾎�����
    FRAME<int>* pt = (FRAME<int>*)lpparameter;
    // �f�o�b�O���[�h�ŃT���v������̓ǂݍ��݁C�����[�X���[�h��camera����̓ǂݍ���
    pt->writer.open(pt->filename, pt->fourcc, pt->fps, cv::Size(pt->width, pt->height));
    while (true) {
        keisoku9 = std::chrono::system_clock::now();
        // �C�x���g�I�u�W�F�N�g���V�O�i����ԂɂȂ�܂ő҂��܂��B
        WaitForSingleObject(start4, INFINITE);
        
        pt->writer << pt->img4;
        SetEvent(hEvent4); // hEvent4���V�O�i����Ԃɂ���
        keisoku10 = std::chrono::system_clock::now();
        stringstream time5;
        time5 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku10 - keisoku9).count()) / 1000; // �����_�Ƃ���������
        cout << "time5: " << time5.str() << endl;
        if (pt->Key == 1) {
            break;
        }
    }
    // �X���b�h�̏I��
    return 0; // 0��Ԃ�
}

int main(int argc, char** argv) {
    //HANDLE CreateThread(
    //    LPSECURITY_ATTRIBUTES      lpThreadAttributes, // CreateThread�֐�����Ԃ��ꂽ�n���h�����`���C���h�v���Z�X�Ɍp���ł��邩�ǂ��������肷��SECURITY_ATTRIBUTES�\���̂ւ̃|�C���^���w��
    //    SIZE_T                                   dwStackSize, // �X�^�b�N�̏����T�C�Y���o�C�g�P�ʂŎw��
    //    LPTHREAD_START_ROUTINE   lpStartAddress, // �X���b�h�Ŏ��s�����֐��ւ̃|�C���^���w��
    //    __drv_aliasesMem LPVOID         lpParameter, // �X���b�h�֐��ɓn���������Ƃ��Ă̕ϐ��ւ̃|�C���^���w��
    //    DWORD                                 dwCreationFlags, // �X���b�h�̍쐬�𐧌䂷��t���O
    //    LPDWORD                              lpThreadId // �쐬���ꂽ�X���b�h��ID���󂯎��ϐ��ւ̃|�C���^���w��
    //);// �߂�l�@�֐������������ꍇ�͐V�����X���b�h�ւ̃n���h���A�֐������s�����ꍇ��NULL

    // �t���[���N���X�̃I�u�W�F�N�g��錾
    FRAME<int> frame;

#ifdef _DEBUG
    cout << "input filename to save:"; // ��O�����~����
    cin >> frame.filename;
    frame.filename += ".mp4";
    cout << frame.filename << endl;
    cout << "Input video path!" << endl;
    cout << "video path:";
    cin >> frame.camID;
    
#else    
    cout << "Input camera ID!" << endl;
    cout << "camera ID:";
    cin >> frame.camID; // ��O�����K�v
    cout << "input filename to save:"; // ��O�����~����
    cin >> frame.filename;
    frame.filename += ".mp4";
    cout << frame.filename << endl;    
#endif /*_DEBUG OR RELEASE*/

    frame.cap.open(frame.camID);
    frame.width = (int)frame.cap.get(cv::CAP_PROP_FRAME_WIDTH);	// �t���[���������擾
    frame.height = (int)frame.cap.get(cv::CAP_PROP_FRAME_HEIGHT);	// �t���[���c�����擾
    frame.fps = frame.cap.get(cv::CAP_PROP_FPS);
   
    //---------------�猟�o�p�̃t���[�����[�N����w�K���f����ǂݍ���------------------------------------------------------------------
    // ImageNet Caffe���t�@�����X���f�� �K�v�ȃ��f�������[�h
    const std::string caffeConfigFile = "D:/ss/ss07/ss07/ss07/dnn(caffe)/deploy.prototxt";
    const std::string caffeWeightFile = "D:/ss/ss07/ss07/ss07/dnn(caffe)/res10_300x300_ssd_iter_140000_fp16.caffemodel";


    // net�Ƀ��f����ǂݍ��߂Ă��Ȃ��������O�����ŏI�� �������������Ƃɏ����������ق������������H
    try {
        frame.net = cv::dnn::readNetFromCaffe(caffeConfigFile, caffeWeightFile);
    }
    catch (cv::Exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        if (frame.net.empty())
        {
            //end = std::chrono::system_clock::now();  // �v���I������
            return -1;
        }
    }
    // �z��̐錾
    
    HANDLE hThread[5]; // HANDLE�^�X���b�h�n���h���z��(�v�f��5)
    DWORD dwThread[5]; // HANDLE�^�X���b�hID�z��dwThreadID(�v�f��5) �X���b�h�̊֐�����������ƁC�X���b�h�n���h�����Ԃ�
    HANDLE start[4]; // �e�X���b�h���J�n������
    HANDLE event[4]; // �e�X���b�h�𓯊�������

    // �E�����̐���̍쐬(������s)
    // �C�x���g�I�u�W�F�N�g���쐬����
    start[0] = CreateEvent(NULL, FALSE, FALSE, START1); // NULL, ��V�O�i����� TRUE:�蓮 FALSE:�������Z�b�g, ������� TRUE:�V�O�i����� FALSE:��V�O�i�����
    start[1] = CreateEvent(NULL, FALSE, FALSE, START2); // Whether start Blur
    start[2] = CreateEvent(NULL, FALSE, FALSE, START3); // Whether start Show
    start[3] = CreateEvent(NULL, FALSE, FALSE, START4); // Whether start Save

    // ����������̍쐬
    /*�C�x���g�I�u�W�F�N�g�̍쐬
    �������FNULL
    �������FTRUE���蓮���Z�b�g,FALSE���������Z�b�g�̃I�u�W�F�N�g���쐬
    ��O�����F�C�x���g�I�u�W�F�N�g�̏�����Ԃ��V�O�i�����ǂ���
    ��O�����F�쐬����C�x���g�I�u�W�F�N�g�̖��O*/
    event[0] = CreateEvent(NULL, FALSE, TRUE, EVENT_NAME1); 
    event[1] = CreateEvent(NULL, FALSE, TRUE, EVENT_NAME2);
    event[2] = CreateEvent(NULL, FALSE, TRUE, EVENT_NAME3);
    event[3] = CreateEvent(NULL, FALSE, TRUE, EVENT_NAME4);
    
    // �X���b�h���I���܂ŋx�~
   
    // �J�������̓X���b�h�̊J�n
    hThread[0] = CreateThread(NULL, 0, CapCamera, &frame, 0, &dwThread[0]);
    // �猟�o�X���b�h�̊J�n
    hThread[1] = CreateThread(NULL, 0, DetectFace, &frame, 0, &dwThread[1]);
    // �ڂ����X���b�h�̊J�n
    hThread[2] = CreateThread(NULL, 0, Blur, &frame, 0, &dwThread[2]);
    //-----��ʏo�͂�File�ۑ��͔񓯊����񉻏����\--------------------------
    hThread[3] = CreateThread(NULL, 0, ShowScreen, &frame, 0, &dwThread[3]);
    hThread[4] = CreateThread(NULL, 0, SaveFile, &frame, 0, &dwThread[4]);
//---------------------------------�X���b�h�I������-------------------------------------------------------------------
    WaitForMultipleObjects(5, hThread, TRUE, INFINITE);
    // �J�������̓X���b�h�n���h�������
    CloseHandle(hThread[0]);
    // �猟�o�X���b�h�n���h�������
    CloseHandle(hThread[1]);
    // �ڂ����X���b�h�n���h�������
    CloseHandle(hThread[2]);
    // ��ʏo�̓X���b�h�n���h�������
    CloseHandle(hThread[3]);
    // ��ʏo�̓X���b�h�n���h�������
    CloseHandle(hThread[4]);
    // �v���O�����̏I��
    return 0;

} 

// ���������s�������o�֐��@ �~�`�Ƀu���[���邩�C�猟�o�����͈͂����}�X�N�擾���ău���[�����邩
template<class T>
void FRAME<T>::cvSmooth(cv::Mat& src, cv::Mat& dst, int mode) {
    if (GetAsyncKeyState(VK_F1)) {
        //F1�������ꂽ�Ƃ��̏���
        BlurMode = CV_BLUR;
    }
    if (GetAsyncKeyState(VK_F2)) {
        //F2�������ꂽ�Ƃ��̏���
        BlurMode = CV_GAUSSIAN;
    }
    if (GetAsyncKeyState(VK_F3)) {
        //F3�������ꂽ�Ƃ��̏���
        BlurMode = CV_MEDIAN;
    }
    if (GetAsyncKeyState(VK_F4)) {
        //F4�������ꂽ�Ƃ��̏���
        BlurMode = NONE;
    }
    if (GetAsyncKeyState(VK_UP)) {
        sigma += 10;
    }
    if (GetAsyncKeyState(VK_DOWN)) {
        sigma -= 10;
        if (sigma <= 0) {
            sigma = 0; // sigma���}�C�i�X�ɂȂ�Ȃ�C�����l�ɖ߂�(��O����)
        }
    }
    switch (mode) {
        case CV_GAUSSIAN:
            cv::GaussianBlur(src, src, cv::Size(15, 15), sigma); // src:���́@src:�o��
            cv::GaussianBlur(src, src, cv::Size(15, 15), sigma); // src:���́@src:�o��
            cv::GaussianBlur(src, src, cv::Size(15, 15), sigma); // src:���́@src:�o��
            cv::GaussianBlur(src, src, cv::Size(15, 15), sigma); // src:���́@src:�o��
            cv::GaussianBlur(src, src, cv::Size(15, 15), sigma); // src:���́@src:�o��
            break;
        case CV_BLUR:
            cv::blur(src, src, cv::Size(15, 15)); // src:���́@src:�o�� mode:�t�B���^
            cv::blur(src, src, cv::Size(15, 15)); // src:���́@src:�o�� mode:�t�B���^
            cv::blur(src, src, cv::Size(15, 15)); // src:���́@src:�o�� mode:�t�B���^
            cv::blur(src, src, cv::Size(15, 15)); // src:���́@src:�o�� mode:�t�B���^
            cv::blur(src, src, cv::Size(15, 15)); // src:���́@src:�o�� mode:�t�B���^
            break;
        case CV_MEDIAN:
            cv::medianBlur(src, src, 5); // src:���́@src:�o�� mode:�t�B���^
            cv::medianBlur(src, src, 5); // src:���́@src:�o�� mode:�t�B���^
            cv::medianBlur(src, src, 5); // src:���́@src:�o�� mode:�t�B���^
            cv::medianBlur(src, src, 5); // src:���́@src:�o�� mode:�t�B���^
            cv::medianBlur(src, src, 5); // src:���́@src:�o�� mode:�t�B���^
            break;
        case NONE:
            break;
    }
}

// �t���X�N���[���\���̊֐�
template<class T>
int FRAME<T>::ShowImageFullScreen(cv::Mat &src) {
    // �t���X�N���[���\������
    static const std::string kWinName = "privacy camera";
    cv::namedWindow(kWinName, cv::WINDOW_NORMAL);
    cv::setWindowProperty(kWinName, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
    cv::imshow(kWinName, src);
    // q�L�[�������ꂽ���ʂ����
    if (cv::waitKey(1) == 'q') {
        Key = 1;
    }
    return 0;
}

#else
#include "MyOpenCV.h"
#include <chrono>
#include <iomanip>
#include <vector>

// caffe��300*300�摜����͂Ƃ��ē���
#define inWidth 300
#define inHeight 300

// Blur�p
#define CV_GAUSSIAN 0
#define  CV_BLUR 1
#define CV_MEDIAN 2

// �猟�o�p
#define inScaleFacter  (double)1.0
#define confidenceThreshold (float)0.5

using namespace std;
// ���Ԍv���p(�v���O�����N��������̎��Ԃ��v��)
chrono::system_clock::time_point keisoku1;
chrono::system_clock::time_point keisoku2;
chrono::system_clock::time_point keisoku3;
chrono::system_clock::time_point keisoku4;
chrono::system_clock::time_point keisoku5;
chrono::system_clock::time_point keisoku6;
chrono::system_clock::time_point keisoku7;
chrono::system_clock::time_point keisoku8;
chrono::system_clock::time_point keisoku9;
chrono::system_clock::time_point keisoku10;

// frame�N���X
template <class T>
class FRAME {

public:
    cv::VideoCapture cap;
    cv::Mat img;
    // ���摜�̏c��
    int frameWidth;
    int frameHeight;
    cv::dnn::Net net;
    // �t���[������
    int slide = 0;

    //T faceCoordinates; // ���4�p�̍��W
    // ���������s���֐��@ �~�`�Ƀu���[���邩�C�猟�o�����͈͂����}�X�N�擾���ău���[�����邩
    void cvSmooth(cv::Mat& src, cv::Mat& dst, int mode);

    // ����t�@�C���������o�����߂̃I�u�W�F�N�g��錾����
    cv::VideoWriter writer;
    int    width, height; // �쐬���铮��t�@�C���̐ݒ�
    int fourcc = cv::VideoWriter::fourcc('M', 'P', '4', 'V'); // �r�f�I�t�H�[�}�b�g�̎w��( ISO MPEG-4 / .mp4)
    double fps;

    // ���Ԍv���p(�v���O�����N��������̎��Ԃ��v��)
    chrono::system_clock::time_point start;
};

//---------------------------------------------------------------------------------

int main(int argc, char** argv) {

    // �t���[���N���X�̃I�u�W�F�N�g��錾
    FRAME<int> frame;
    //---------------�猟�o�p�̃t���[�����[�N����w�K���f����ǂݍ���------------------------------------------------------------------
    // ImageNet Caffe���t�@�����X���f�� �K�v�ȃ��f�������[�h
    const std::string caffeConfigFile = "D:/ss/ss07/ss07/ss07/dnn(caffe)/deploy.prototxt";
    const std::string caffeWeightFile = "D:/ss/ss07/ss07/ss07/dnn(caffe)/res10_300x300_ssd_iter_140000_fp16.caffemodel";

    // net�Ƀ��f����ǂݍ��߂Ă��Ȃ��������O�����ŏI�� �������������Ƃɏ����������ق������������H
    try {
        frame.net = cv::dnn::readNetFromCaffe(caffeConfigFile, caffeWeightFile);
    }
    catch (cv::Exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        if (frame.net.empty()) {
            //end = std::chrono::system_clock::now();  // �v���I������
            return -1;
        }
    }
    // �f�o�b�O���[�h�ŃT���v������̓ǂݍ��݁C�����[�X���[�h��camera����̓ǂݍ���
    frame.writer.open("CloneVideo.mp4", frame.fourcc, frame.fps, cv::Size(frame.width, frame.height));
    // �f�o�b�O���[�h�ŃT���v������̓ǂݍ��݁C�����[�X���[�h��camera����̓ǂݍ���
    // �^�悵�������ǂݍ���
#ifdef _DEBUG
    frame.cap.open("d:/ss/ss07/ss07/sample.mp4");
    // �v���O�����J�n����̎��Ԃ��v��
    frame.start = std::chrono::system_clock::now();
    if (frame.cap.isOpened() == false) {
        // ����t�@�C�����J���Ȃ������Ƃ��͏I������
        cerr << "can't load video" << endl;
        return -1;
    }
#else /*RELEASE*/
    frame.cap.open(0); // �f�t�H���g�J�������I�[�v��
    frame.start = std::chrono::system_clock::now();
    if (!frame.cap.isOpened()) {  // �����������ǂ������`�F�b�N
        cerr << "can't open video!" << endl;
        return -1;
    }
#endif /*_DEBUG OR RELEASE*/
    while (true) {
        keisoku1 = std::chrono::system_clock::now();
        frame.cap >> frame.img; //USB�J��������������̂P�t���[�����i�[    
        keisoku2 = std::chrono::system_clock::now();
        stringstream time1;
        time1 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku2 - keisoku1).count()) / 1000; // �����_�Ƃ���������
        cout << "time1: " << time1.str() << endl;
        // ���摜�̏c���̃T�C�Y(�A�m�e�[�V�������ʂ��������΂��Ƃ��̃T�C�Y�v�Z�p)
        frame.frameWidth = frame.img.cols;
        frame.frameHeight = frame.img.rows;

        // ����ۑ��Ɖ�ʕ\���̍ۂ̏����擾
        frame.width = (int)frame.cap.get(cv::CAP_PROP_FRAME_WIDTH);	// �t���[���������擾
        frame.height = (int)frame.cap.get(cv::CAP_PROP_FRAME_HEIGHT);	// �t���[���c�����擾
        frame.fps = frame.cap.get(cv::CAP_PROP_FPS);				// �t���[�����[�g���擾

        //----------------�O����-----------------------------------------------------------------------------------------------
        cv::Mat inputBlob = cv::dnn::blobFromImage(frame.img, 0.5, cv::Size(inWidth, inHeight), false, false);
        frame.net.setInput(inputBlob); // net��blob��n��
        cv::Mat detection = frame.net.forward(); //4-dim:nchw
        // N: Number of detected objects as face
        // C: color channels
        // H: Height
        // W: Width
        //----------------------------------------------------------------��������猟�m--------------------------------------------------------------------
        cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
        // detection.size[2]: �m�M�x
        // detection.size[3]: x1, y1, x2, y2�E�E�E
        // �t���[�����ƂɐV����vector��ݒ肷��
        int x1;
        int y1;
        int x2;
        int y2;

        int j = 0;
        keisoku3 = std::chrono::system_clock::now();
        for (int i = 0; i < detectionMat.rows; i++)
        {
            float confidence = detectionMat.at<float>(i, 2); // ���o�������ʂ̐M���x�̉����l
            // ��ƌ��o����
            if (confidence > confidenceThreshold)
            {
                int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frame.frameWidth);
                int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frame.frameHeight);
                int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frame.frameWidth);
                int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frame.frameHeight);
                // img�ɐԘg��������(�Ԙg�\���E��\����؂�ւ�����悤�ɂ���) �R�}���h���C���p�[�T�[�ł�����
                // �t���[���J�E���g�C�^�C���X�^���v�̕\���E��\���؂�ւ�
    //-----------------------------------------���o������ɐԘg��������C���o����\��(�؂�ւ���悤�ɂ���)--------------------------------------------------------
                cv::rectangle(frame.img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255), 2, 4);
                keisoku5 = std::chrono::system_clock::now();
                cv::Mat roi = cv::Mat(frame.img, cv::Rect(x1, y1, x2 - x1, y2 - y1));
                frame.cvSmooth(roi, roi, CV_GAUSSIAN);
                stringstream ss;
                ss << fixed << setprecision(2) << confidence * 100;

                cv::putText(
                    frame.img,
                    ss.str() + "%",
                    cv::Point(x1, y1),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.5,
                    cv::Scalar(0, 0, 255),
                    1
                );
                keisoku6 = std::chrono::system_clock::now();
                j++;
            }
        }
        keisoku4 = std::chrono::system_clock::now();
        stringstream time2;
        time2 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku4 - keisoku3).count()) / 1000; // �����_�Ƃ���������
        cout << "time2: " << time2.str() << endl;
        // �f�o�b�O���[�h�ŃT���v������̓ǂݍ��݁C�����[�X���[�h��camera����̓ǂݍ���
        // �t���[�����̕\��
        frame.slide += 1;
        keisoku7 = std::chrono::system_clock::now();
        cv::putText(
            frame.img,
            "Frame:" + to_string(frame.slide),
            cv::Point(6, 15),
            cv::FONT_HERSHEY_SIMPLEX,
            0.5,
            cv::Scalar(0, 0, 255),
            1
        );
        // �v���O�����N��������̎��Ԃ�\��
        auto end = std::chrono::system_clock::now();  // �v���O�����N������̌v���I������
        auto diff = end - frame.start; // �o�ߎ���
        stringstream time;
        time << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::microseconds>(diff).count()) / 1000000; // �����_�Ƃ���������
        cv::putText(
            frame.img,
            "time:" + time.str() + " [s]",
            cv::Point(100, 15),
            cv::FONT_HERSHEY_SIMPLEX,
            0.5,
            cv::Scalar(0, 0, 255),
            1
        );
        // �t���X�N���[���\������
        static const std::string kWinName = "privacy camera";
        cv::namedWindow(kWinName, cv::WINDOW_NORMAL);
        cv::setWindowProperty(kWinName, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
        cv::imshow(kWinName, frame.img);
        keisoku8 = std::chrono::system_clock::now();
        stringstream time4;
        time4 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku8 - keisoku7).count()) / 1000; // �����_�Ƃ���������
        cout << "time4: " << time4.str() << endl;
        keisoku9 = std::chrono::system_clock::now();
        frame.writer << frame.img;
        keisoku10 = std::chrono::system_clock::now();
        stringstream time5;
        time5 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku10 - keisoku9).count()) / 1000; // �����_�Ƃ���������
        cout << "time5: " << time5.str() << endl;
        // q�L�[�������ꂽ���ʂ����
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }
    // �v���O�����̏I��
    return 0;

}

// ���������s�������o�֐��@ �~�`�Ƀu���[���邩�C�猟�o�����͈͂����}�X�N�擾���ău���[�����邩
template<class T>
void FRAME<T>::cvSmooth(cv::Mat& src, cv::Mat& dst, int mode) {
    cv::Mat src_roi1;
    cv::Mat src_roi2;
    /*cv::Mat roi;*/
    switch (mode) {
    case CV_GAUSSIAN:

        cv::GaussianBlur(src, src, cv::Size(0, 0), 10); // src:���́@src:�o��
        /*cout << "GaussianBlur" << endl;*/
        src_roi1 = cv::Mat(src, cv::Rect(src.cols / 4, src.rows / 4, src.cols - src.cols / 2, src.rows - src.rows / 2)); // ����̒��_����؂��镝�ƍ���
        cv::GaussianBlur(src_roi1, src_roi1, cv::Size(0, 0), 10);
        //cout << "GaussianBlur" << endl;
        src_roi2 = cv::Mat(src, cv::Rect(src.cols / 3, src.rows / 3, src.cols / 3, src.rows / 3)); // ����̒��_����؂��镝�ƍ���
        cv::GaussianBlur(src_roi2, src_roi2, cv::Size(0, 0), 10);
        //cout << "GaussianBlur" << endl;
        break;
    case CV_BLUR:
        cv::blur(src, src, cv::Size(5, 5)); // src:���́@src:�o�� mode:�t�B���^
        src_roi1 = cv::Mat(src, cv::Rect(src.cols / 4, src.rows / 4, src.cols - src.cols / 2, src.rows - src.rows / 2)); // ����̒��_����؂��镝�ƍ���
        cv::blur(src_roi1, src_roi1, cv::Size(5, 5));
        src_roi2 = cv::Mat(src, cv::Rect(src.cols / 3, src.rows / 3, src.cols / 3, src.rows / 3)); // ����̒��_����؂��镝�ƍ���
        cv::blur(src_roi2, src_roi2, cv::Size(5, 5));
        break;
    case CV_MEDIAN:
        cv::medianBlur(src, src, 5); // src:���́@src:�o�� mode:�t�B���^
        src_roi1 = cv::Mat(src, cv::Rect(src.cols / 4, src.rows / 4, src.cols - src.cols / 2, src.rows - src.rows / 2)); // ����̒��_����؂��镝�ƍ���
        cv::medianBlur(src_roi1, src_roi1, 5);
        src_roi2 = cv::Mat(src, cv::Rect(src.cols / 3, src.rows / 3, src.cols / 3, src.rows / 3)); // ����̒��_����؂��镝�ƍ���
        cv::medianBlur(src_roi2, src_roi2, 5);
        break;
    }
}

#endif
