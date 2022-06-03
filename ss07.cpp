#define pipeline
// パイプラインと逐次処理を比較する為の条件分岐
#ifdef pipeline

#include "MyOpenCV.h"
#include <chrono>
#include <iomanip>
#include <vector>



// caffeは300*300画像を入力として得る
#define inWidth 300
#define inHeight 300

// Blur用
#define CV_GAUSSIAN 0
#define  CV_BLUR 1
#define CV_MEDIAN 2
#define NONE 3

// 顔検出用
#define inScaleFacter  (double)1.0
#define confidenceThreshold (float)0.5

// ループブレイク用
#define STOP 1

// イベントオブジェクト名
#define EVENT_NAME1  L"EVENT1"
#define EVENT_NAME2  L"EVENT2"
#define EVENT_NAME3  L"EVENT3"
#define EVENT_NAME4  L"EVENT4"

// 後段スレッドを同期させる(初回ループ)
#define START1  L"START1"
#define START2  L"START2"
#define START3  L"START3"
#define START4  L"START4"

using namespace std;

// 時間計測用(プログラム起動時からの時間を計測)
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

// frameクラス
template <class T>
class FRAME{
    
public:
    cv::VideoCapture cap;
    cv::Mat img;
    cv::Mat img2;
    cv::Mat img3;
    cv::Mat img4;
    // 元画像の縦横
    int frameWidth;
    int frameHeight;
    /*cv::dnn::Net net;*/
    // フレーム枚数
    int slide = 0;
    // 無限ループ終了
    int Key = 0;
#ifdef _DEBUG
    // video path
    string camID = "D:/ss/ss07/ss07/sample.MP4";
#else
    // camera ID
    int camID = 0;

#endif
    // ブラーオプション
    int BlurMode = CV_GAUSSIAN;
    // ブラー強度
    int sigma = 0;

    // rect_change
    bool rectChange = true;

    // net
    cv::dnn::Net net;

    // 1フレームに検出した顔(の2角の座標)をすべて格納する動的な配列(vector)
    vector<T> detectedFacesX1copy;
    vector<T> detectedFacesY1copy;
    vector<T> detectedFacesX2copy;
    vector<T> detectedFacesY2copy;

    //T faceCoordinates; // 顔の4角の座標
    // 平滑化を行う関数　 円形にブラーするか，顔検出した範囲から顔マスク取得してブラーかけるか
    void cvSmooth(cv::Mat& src, cv::Mat& dst, int mode);
    // フルスクリーン表示の関数
    int ShowImageFullScreen(cv::Mat& src);

    // 動画ファイルを書き出すためのオブジェクトを宣言する
    cv::VideoWriter writer;
    int    width, height; // 作成する動画ファイルの設定
    int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v'); // ビデオフォーマットの指定( ISO MPEG-4 / .mp4)
    float fps;
    // 保存動画ファイル名
    string filename;

    // 時間計測用(プログラム起動時からの時間を計測)
    chrono::system_clock::time_point start;
};

//---------------------------------------------------------------------------------

DWORD WINAPI CapCamera(LPVOID lpparameter) {
    // デバッグモードでサンプル動画の読み込み，リリースモードでcamera動画の読み込み
    // 録画した動画を読み込み
    // 後段スレッドの開始イベント
    // イベントオブジェクトを取得します
   /*イベントオブジェクトをオープンする．
   第一引数：オープンするイベントオブジェクトに対してどのようなアクセスを行うか指定するフラグ
   第二引数：取得したハンドルが継承可能か否かを表す
   第三引数：オープンするイベントオブジェクトの名前
   戻り値：オープンしたイベントの戻り値(失敗→NULL)*/
    HANDLE start1 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START1);
    HANDLE hEvent1 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME1);
   // ポインタであることを明示する
    FRAME<int>* pt = (FRAME<int>*)lpparameter;
#ifdef _DEBUG
    pt->cap.open(pt->camID);
    // プログラム開始からの時間を計測
    pt->start = std::chrono::system_clock::now();
    if (pt->cap.isOpened() == false) {
    // 動画ファイルが開けなかったときは終了する
        cerr << "can't load video" << endl;
        return -1;
    }
#else /*RELEASE*/
    
    pt->cap.open(pt->camID); // デフォルトカメラをオープン
    pt->start = std::chrono::system_clock::now();
    if (!pt->cap.isOpened()) {  // 成功したかどうかをチェック
        cerr << "can't open video!" << endl;
        return -1;
    }
#endif /*_DEBUG OR RELEASE*/
    while (true) {
        keisoku1 = std::chrono::system_clock::now();
        pt->cap >> pt->img; //USBカメラが得た動画の１フレームを格納    
        
        // 元画像の縦横のサイズ(アノテーション部位を引き延ばすときのサイズ計算用)
        pt->frameWidth = pt->img.cols;
        pt->frameHeight = pt->img.rows;

        // 動画保存と画面表示の際の情報を取得
        pt->width = (int)pt->cap.get(cv::CAP_PROP_FRAME_WIDTH);	// フレーム横幅を取得
        pt->height = (int)pt->cap.get(cv::CAP_PROP_FRAME_HEIGHT);	// フレーム縦幅を取得
        pt->fps = (float)pt->cap.get(cv::CAP_PROP_FPS);				// フレームレートを取得
        
        // イベントオブジェクトがシグナル状態になるまで待ちます
        WaitForSingleObject(hEvent1, INFINITE); 
        // 後ろのスレッドに渡す変数
        pt->img2 = pt->img.clone(); // コピーを渡す
        // 一回目のカメラ入力があれば，後段スレッドのループを開始させる
        SetEvent(start1); // START1イベント(顔検出スレッド)をシグナル状態にする
        keisoku2 = std::chrono::system_clock::now();
        stringstream time1;
        time1 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku2 - keisoku1).count()) / 1000; // 小数点とか直したい
        cout << "time1: " << time1.str() << endl;
        if (pt->Key == 1) {
            break;
        }
    }
    // スレッドの終了
    return 0; // 0を返す
}

DWORD WINAPI DetectFace(LPVOID lpparameter) {
    
    // ポインタであることを明示する
    FRAME<int>* pt = (FRAME<int>*)lpparameter;
    
    //------------------------------------------------------------------------------------------------------------------
    // 顔検出スレッドの開始イベント
    HANDLE start1 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START1);
    // ぼかしスレッドの開始イベント
    HANDLE start2 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START2);
    
    // イベントオブジェクトの取得 
    HANDLE hEvent2 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME2);
    // イベントオブジェクトを取得
    HANDLE hEvent1 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME1);
    // イベントオブジェクトがシグナル状態になるまで待ちます。
   
    // デバッグモードでサンプル動画の読み込み，リリースモードでcamera動画の読み込み
    // イメージはBlobに変換され、forward() 関数を使用してネットワークを通過 前処理
    // 1.平均減算とスケーリングを調べる(オプションでチャンネルスワッピング)
   
    while (true) {
        keisoku3 = std::chrono::system_clock::now();
        WaitForSingleObject(start1, INFINITE); // img2に画像が入力されるまで待つ
        //----------------前処理-----------------------------------------------------------------------------------------------
        cv::Mat inputBlob = cv::dnn::blobFromImage(pt->img2, 0.5, cv::Size(inWidth, inHeight), false, false);
        pt->net.setInput(inputBlob); // netにblobを渡す
        cv::Mat detection = pt->net.forward(); //4-dim:nchw
        // N: Number of detected objects as face
        // C: color channels
        // H: Height
        // W: Width
    //----------------------------------------------------------------ここから顔検知--------------------------------------------------------------------
        cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
        // detection.size[2]: 確信度
        // detection.size[3]: x1, y1, x2, y2・・・
        // フレームごとに新しくvectorを設定する
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
            float confidence = detectionMat.at<float>(i, 2); // 検出した部位の信頼度の下限値
            // 顔と検出する
            if (confidence > confidenceThreshold)
            {
                detectedFacesX1.push_back(static_cast<int>(detectionMat.at<float>(i, 3) * pt->frameWidth));
                detectedFacesY1.push_back(static_cast<int>(detectionMat.at<float>(i, 4) * pt->frameHeight));
                detectedFacesX2.push_back(static_cast<int>(detectionMat.at<float>(i, 5) * pt->frameWidth));
                detectedFacesY2.push_back(static_cast<int>(detectionMat.at<float>(i, 6) * pt->frameHeight));
                // imgに赤枠をかける(赤枠表示・非表示を切り替えられるようにする) コマンドラインパーサーでしたい
                // フレームカウント，タイムスタンプの表示・非表示切り替え
//-----------------------------------------検出した顔に赤枠をかける，検出率を表示(切り替えるようにする)--------------------------------------------------------
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

        //-----------------------------------------------ここまで顔検出-----------------------------------------------------------------------------------------------
        // イベントオブジェクトがシグナル状態になるまで待ちます。
        WaitForSingleObject(hEvent2, INFINITE); // ぼかしスレッドを待つ
        pt->img3 = pt->img2.clone(); // コピーを渡す
        // カメラ入力スレッドを同期させる
        SetEvent(hEvent1);
        // vectorのコピーを渡す
        std::copy(detectedFacesX1.begin(), detectedFacesX1.end(), std::back_inserter(pt->detectedFacesX1copy));
        std::copy(detectedFacesY1.begin(), detectedFacesY1.end(), std::back_inserter(pt->detectedFacesY1copy));
        std::copy(detectedFacesX2.begin(), detectedFacesX2.end(), std::back_inserter(pt->detectedFacesX2copy));
        std::copy(detectedFacesY2.begin(), detectedFacesY2.end(), std::back_inserter(pt->detectedFacesY2copy));
        // Blurスレッドに渡すメモリが格納し終えたので，(初回ループ)スタートイベントをシグナルにする
        SetEvent(start2); // start2をシグナル状態にする
        keisoku4 = std::chrono::system_clock::now();
        stringstream time2;
        time2 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku4 - keisoku3).count()) / 1000; // 小数点とか直したい
        cout << "time2: " << time2.str() << endl;
        if (pt->Key == 1) {
            break;
        }
    } 
    // スレッドの終了
    return 0; // 0を返す
}

DWORD WINAPI Blur(LPVOID lpparameter) {
    // ポインタであることを明示する
    FRAME<int>* pt = (FRAME<int>*)lpparameter;
    // ぼかしスレッドの開始イベント
    HANDLE start2 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START2);
    // 表示・保存スレッドの開始イベント
    HANDLE start3 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START3);
    // 表示・保存スレッドの開始イベント
    HANDLE start4 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START4);


    // イベントオブジェクトの取得 // 表示スレッドを待つ
    HANDLE hEvent3 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME3);
    // イベントオブジェクトの取得 // 保存スレッドを待つ
    HANDLE hEvent4 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME4);

    // イベントオブジェクトの取得 // 検出スレッドをシグナル化させる
    HANDLE hEvent2 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME2);
   
    while (true) {
        keisoku5 = std::chrono::system_clock::now();
        // イベントオブジェクトがシグナル状態になるまで待ちます
        WaitForSingleObject(start2, INFINITE);

        // デバッグモードでサンプル動画の読み込み，リリースモードでcamera動画の読み込み
        int j = 0;
        for (auto& entry : pt->detectedFacesX1copy) {
            cv::Mat roi = cv::Mat(pt->img3, cv::Rect(pt->detectedFacesX1copy[j], pt->detectedFacesY1copy[j], pt->detectedFacesX2copy[j] - pt->detectedFacesX1copy[j], pt->detectedFacesY2copy[j] - pt->detectedFacesY1copy[j]));
            //-------------------------------------------------------ブラーをかける------------------------------------------------------------------------------------------
            pt->cvSmooth(roi, roi, pt->BlurMode); // ガウシアンブラーで平滑化が行われる 第三引数でブラーを切り替えられる
            
            j++;
        }
        // 表示と保存を待つ
        WaitForSingleObject(hEvent3, INFINITE);
        WaitForSingleObject(hEvent4, INFINITE);
        pt->img4 = pt->img3.clone();
       
        SetEvent(hEvent2); // detectをシグナル状態にする
        // 不要なメモリを開放
        pt->detectedFacesX1copy.clear();
        pt->detectedFacesY1copy.clear();
        pt->detectedFacesX2copy.clear();
        pt->detectedFacesY2copy.clear();
        SetEvent(start3); // 表示スレッドをシグナル化させる
        SetEvent(start4); // 保存スレッドをシグナル化させる
        keisoku6 = std::chrono::system_clock::now();
        stringstream time3;
        time3 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku6 - keisoku5).count()) / 1000; // 小数点とか直したい
        cout << "time3: " << time3.str() << endl;
        if (pt->Key == 1) {
            break;
        }
    }
    // スレッドの終了
    return 0; // 0を返す
}

DWORD WINAPI ShowScreen(LPVOID lpparameter) {
    // ポインタであることを明示する
    FRAME<int>* pt = (FRAME<int>*)lpparameter;
    // 表示・保存スレッドの開始イベント
    HANDLE start3 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START3);

    // イベントオブジェクトの取得 // 表示・保存スレッドを待つ
    HANDLE hEvent3 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME3);
    
    while (true) {
        keisoku7 = std::chrono::system_clock::now();
        // イベントオブジェクトがシグナル状態になるまで待ちます。
        WaitForSingleObject(start3, INFINITE);
        // デバッグモードでサンプル動画の読み込み，リリースモードでcamera動画の読み込み
        // フレーム数の表示
        pt->slide += 1;
        // BaskSpaceキーを押したら，時間などを表示しない
        // プログラム起動時からの時間を表示
        auto end = std::chrono::system_clock::now();  // プログラム起動からの計測終了時間
        auto diff = end - pt->start; // 経過時間
        stringstream time;
        time << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(diff).count()) / 1000; // 小数点とか直したい
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
        
        // マウスの左ボタンでヘルプ表示 
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
        SetEvent(hEvent3); // hEvent3をシグナル状態にする
        keisoku8 = std::chrono::system_clock::now();
        stringstream time4;
        time4 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku8 - keisoku7).count()) / 1000; // 小数点とか直したい
        cout << "time4: " << time4.str() << endl;
        if (pt->Key == 1) {
            break;
        }
    }
    // スレッドの終了
    return 0; // 0を返す
}

DWORD WINAPI SaveFile(LPVOID lpparameter) {

    // 表示・保存スレッドの開始イベント
    HANDLE start4 = OpenEvent(EVENT_ALL_ACCESS, FALSE, START4);
    // イベントオブジェクトの取得 // 表示・保存スレッドを待つ
    HANDLE hEvent4 = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME4);
   
    // ポインタであることを明示する
    FRAME<int>* pt = (FRAME<int>*)lpparameter;
    // デバッグモードでサンプル動画の読み込み，リリースモードでcamera動画の読み込み
    pt->writer.open(pt->filename, pt->fourcc, pt->fps, cv::Size(pt->width, pt->height));
    while (true) {
        keisoku9 = std::chrono::system_clock::now();
        // イベントオブジェクトがシグナル状態になるまで待ちます。
        WaitForSingleObject(start4, INFINITE);
        
        pt->writer << pt->img4;
        SetEvent(hEvent4); // hEvent4をシグナル状態にする
        keisoku10 = std::chrono::system_clock::now();
        stringstream time5;
        time5 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku10 - keisoku9).count()) / 1000; // 小数点とか直したい
        cout << "time5: " << time5.str() << endl;
        if (pt->Key == 1) {
            break;
        }
    }
    // スレッドの終了
    return 0; // 0を返す
}

int main(int argc, char** argv) {
    //HANDLE CreateThread(
    //    LPSECURITY_ATTRIBUTES      lpThreadAttributes, // CreateThread関数から返されたハンドルをチャイルドプロセスに継承できるかどうかを決定するSECURITY_ATTRIBUTES構造体へのポインタを指定
    //    SIZE_T                                   dwStackSize, // スタックの初期サイズをバイト単位で指定
    //    LPTHREAD_START_ROUTINE   lpStartAddress, // スレッドで実行される関数へのポインタを指定
    //    __drv_aliasesMem LPVOID         lpParameter, // スレッド関数に渡される引数としての変数へのポインタを指定
    //    DWORD                                 dwCreationFlags, // スレッドの作成を制御するフラグ
    //    LPDWORD                              lpThreadId // 作成されたスレッドのIDを受け取る変数へのポインタを指定
    //);// 戻り値　関数が成功した場合は新しいスレッドへのハンドル、関数が失敗した場合はNULL

    // フレームクラスのオブジェクトを宣言
    FRAME<int> frame;

#ifdef _DEBUG
    cout << "input filename to save:"; // 例外処理欲しい
    cin >> frame.filename;
    frame.filename += ".mp4";
    cout << frame.filename << endl;
    cout << "Input video path!" << endl;
    cout << "video path:";
    cin >> frame.camID;
    
#else    
    cout << "Input camera ID!" << endl;
    cout << "camera ID:";
    cin >> frame.camID; // 例外処理必要
    cout << "input filename to save:"; // 例外処理欲しい
    cin >> frame.filename;
    frame.filename += ".mp4";
    cout << frame.filename << endl;    
#endif /*_DEBUG OR RELEASE*/

    frame.cap.open(frame.camID);
    frame.width = (int)frame.cap.get(cv::CAP_PROP_FRAME_WIDTH);	// フレーム横幅を取得
    frame.height = (int)frame.cap.get(cv::CAP_PROP_FRAME_HEIGHT);	// フレーム縦幅を取得
    frame.fps = frame.cap.get(cv::CAP_PROP_FPS);
   
    //---------------顔検出用のフレームワークから学習モデルを読み込む------------------------------------------------------------------
    // ImageNet Caffeリファレンスモデル 必要なモデルをロード
    const std::string caffeConfigFile = "D:/ss/ss07/ss07/ss07/dnn(caffe)/deploy.prototxt";
    const std::string caffeWeightFile = "D:/ss/ss07/ss07/ss07/dnn(caffe)/res10_300x300_ssd_iter_140000_fp16.caffemodel";


    // netにモデルを読み込めていなかったら例外処理で終了 ここも処理ごとに書き換えたほうがいい部分？
    try {
        frame.net = cv::dnn::readNetFromCaffe(caffeConfigFile, caffeWeightFile);
    }
    catch (cv::Exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        if (frame.net.empty())
        {
            //end = std::chrono::system_clock::now();  // 計測終了時間
            return -1;
        }
    }
    // 配列の宣言
    
    HANDLE hThread[5]; // HANDLE型スレッドハンドル配列(要素数5)
    DWORD dwThread[5]; // HANDLE型スレッドID配列dwThreadID(要素数5) スレッドの関数が成功すると，スレッドハンドルが返る
    HANDLE start[4]; // 各スレッドを開始させる
    HANDLE event[4]; // 各スレッドを同期させる

    // 右向きの制御の作成(初回実行)
    // イベントオブジェクトを作成する
    start[0] = CreateEvent(NULL, FALSE, FALSE, START1); // NULL, 非シグナル状態 TRUE:手動 FALSE:自動リセット, 初期状態 TRUE:シグナル状態 FALSE:非シグナル状態
    start[1] = CreateEvent(NULL, FALSE, FALSE, START2); // Whether start Blur
    start[2] = CreateEvent(NULL, FALSE, FALSE, START3); // Whether start Show
    start[3] = CreateEvent(NULL, FALSE, FALSE, START4); // Whether start Save

    // 左向き制御の作成
    /*イベントオブジェクトの作成
    第一引数：NULL
    第二引数：TRUE→手動リセット,FALSE→自動リセットのオブジェクトが作成
    第三引数：イベントオブジェクトの初期状態がシグナルかどうか
    第三引数：作成するイベントオブジェクトの名前*/
    event[0] = CreateEvent(NULL, FALSE, TRUE, EVENT_NAME1); 
    event[1] = CreateEvent(NULL, FALSE, TRUE, EVENT_NAME2);
    event[2] = CreateEvent(NULL, FALSE, TRUE, EVENT_NAME3);
    event[3] = CreateEvent(NULL, FALSE, TRUE, EVENT_NAME4);
    
    // スレッドが終わるまで休止
   
    // カメラ入力スレッドの開始
    hThread[0] = CreateThread(NULL, 0, CapCamera, &frame, 0, &dwThread[0]);
    // 顔検出スレッドの開始
    hThread[1] = CreateThread(NULL, 0, DetectFace, &frame, 0, &dwThread[1]);
    // ぼかしスレッドの開始
    hThread[2] = CreateThread(NULL, 0, Blur, &frame, 0, &dwThread[2]);
    //-----画面出力とFile保存は非同期並列化処理可能--------------------------
    hThread[3] = CreateThread(NULL, 0, ShowScreen, &frame, 0, &dwThread[3]);
    hThread[4] = CreateThread(NULL, 0, SaveFile, &frame, 0, &dwThread[4]);
//---------------------------------スレッド終了処理-------------------------------------------------------------------
    WaitForMultipleObjects(5, hThread, TRUE, INFINITE);
    // カメラ入力スレッドハンドルを閉じる
    CloseHandle(hThread[0]);
    // 顔検出スレッドハンドルを閉じる
    CloseHandle(hThread[1]);
    // ぼかしスレッドハンドルを閉じる
    CloseHandle(hThread[2]);
    // 画面出力スレッドハンドルを閉じる
    CloseHandle(hThread[3]);
    // 画面出力スレッドハンドルを閉じる
    CloseHandle(hThread[4]);
    // プログラムの終了
    return 0;

} 

// 平滑化を行うメンバ関数　 円形にブラーするか，顔検出した範囲から顔マスク取得してブラーかけるか
template<class T>
void FRAME<T>::cvSmooth(cv::Mat& src, cv::Mat& dst, int mode) {
    if (GetAsyncKeyState(VK_F1)) {
        //F1が押されたときの処理
        BlurMode = CV_BLUR;
    }
    if (GetAsyncKeyState(VK_F2)) {
        //F2が押されたときの処理
        BlurMode = CV_GAUSSIAN;
    }
    if (GetAsyncKeyState(VK_F3)) {
        //F3が押されたときの処理
        BlurMode = CV_MEDIAN;
    }
    if (GetAsyncKeyState(VK_F4)) {
        //F4が押されたときの処理
        BlurMode = NONE;
    }
    if (GetAsyncKeyState(VK_UP)) {
        sigma += 10;
    }
    if (GetAsyncKeyState(VK_DOWN)) {
        sigma -= 10;
        if (sigma <= 0) {
            sigma = 0; // sigmaがマイナスになるなら，初期値に戻す(例外処理)
        }
    }
    switch (mode) {
        case CV_GAUSSIAN:
            cv::GaussianBlur(src, src, cv::Size(15, 15), sigma); // src:入力　src:出力
            cv::GaussianBlur(src, src, cv::Size(15, 15), sigma); // src:入力　src:出力
            cv::GaussianBlur(src, src, cv::Size(15, 15), sigma); // src:入力　src:出力
            cv::GaussianBlur(src, src, cv::Size(15, 15), sigma); // src:入力　src:出力
            cv::GaussianBlur(src, src, cv::Size(15, 15), sigma); // src:入力　src:出力
            break;
        case CV_BLUR:
            cv::blur(src, src, cv::Size(15, 15)); // src:入力　src:出力 mode:フィルタ
            cv::blur(src, src, cv::Size(15, 15)); // src:入力　src:出力 mode:フィルタ
            cv::blur(src, src, cv::Size(15, 15)); // src:入力　src:出力 mode:フィルタ
            cv::blur(src, src, cv::Size(15, 15)); // src:入力　src:出力 mode:フィルタ
            cv::blur(src, src, cv::Size(15, 15)); // src:入力　src:出力 mode:フィルタ
            break;
        case CV_MEDIAN:
            cv::medianBlur(src, src, 5); // src:入力　src:出力 mode:フィルタ
            cv::medianBlur(src, src, 5); // src:入力　src:出力 mode:フィルタ
            cv::medianBlur(src, src, 5); // src:入力　src:出力 mode:フィルタ
            cv::medianBlur(src, src, 5); // src:入力　src:出力 mode:フィルタ
            cv::medianBlur(src, src, 5); // src:入力　src:出力 mode:フィルタ
            break;
        case NONE:
            break;
    }
}

// フルスクリーン表示の関数
template<class T>
int FRAME<T>::ShowImageFullScreen(cv::Mat &src) {
    // フルスクリーン表示する
    static const std::string kWinName = "privacy camera";
    cv::namedWindow(kWinName, cv::WINDOW_NORMAL);
    cv::setWindowProperty(kWinName, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
    cv::imshow(kWinName, src);
    // qキーが押されたら画面を閉じる
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

// caffeは300*300画像を入力として得る
#define inWidth 300
#define inHeight 300

// Blur用
#define CV_GAUSSIAN 0
#define  CV_BLUR 1
#define CV_MEDIAN 2

// 顔検出用
#define inScaleFacter  (double)1.0
#define confidenceThreshold (float)0.5

using namespace std;
// 時間計測用(プログラム起動時からの時間を計測)
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

// frameクラス
template <class T>
class FRAME {

public:
    cv::VideoCapture cap;
    cv::Mat img;
    // 元画像の縦横
    int frameWidth;
    int frameHeight;
    cv::dnn::Net net;
    // フレーム枚数
    int slide = 0;

    //T faceCoordinates; // 顔の4角の座標
    // 平滑化を行う関数　 円形にブラーするか，顔検出した範囲から顔マスク取得してブラーかけるか
    void cvSmooth(cv::Mat& src, cv::Mat& dst, int mode);

    // 動画ファイルを書き出すためのオブジェクトを宣言する
    cv::VideoWriter writer;
    int    width, height; // 作成する動画ファイルの設定
    int fourcc = cv::VideoWriter::fourcc('M', 'P', '4', 'V'); // ビデオフォーマットの指定( ISO MPEG-4 / .mp4)
    double fps;

    // 時間計測用(プログラム起動時からの時間を計測)
    chrono::system_clock::time_point start;
};

//---------------------------------------------------------------------------------

int main(int argc, char** argv) {

    // フレームクラスのオブジェクトを宣言
    FRAME<int> frame;
    //---------------顔検出用のフレームワークから学習モデルを読み込む------------------------------------------------------------------
    // ImageNet Caffeリファレンスモデル 必要なモデルをロード
    const std::string caffeConfigFile = "D:/ss/ss07/ss07/ss07/dnn(caffe)/deploy.prototxt";
    const std::string caffeWeightFile = "D:/ss/ss07/ss07/ss07/dnn(caffe)/res10_300x300_ssd_iter_140000_fp16.caffemodel";

    // netにモデルを読み込めていなかったら例外処理で終了 ここも処理ごとに書き換えたほうがいい部分？
    try {
        frame.net = cv::dnn::readNetFromCaffe(caffeConfigFile, caffeWeightFile);
    }
    catch (cv::Exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        if (frame.net.empty()) {
            //end = std::chrono::system_clock::now();  // 計測終了時間
            return -1;
        }
    }
    // デバッグモードでサンプル動画の読み込み，リリースモードでcamera動画の読み込み
    frame.writer.open("CloneVideo.mp4", frame.fourcc, frame.fps, cv::Size(frame.width, frame.height));
    // デバッグモードでサンプル動画の読み込み，リリースモードでcamera動画の読み込み
    // 録画した動画を読み込み
#ifdef _DEBUG
    frame.cap.open("d:/ss/ss07/ss07/sample.mp4");
    // プログラム開始からの時間を計測
    frame.start = std::chrono::system_clock::now();
    if (frame.cap.isOpened() == false) {
        // 動画ファイルが開けなかったときは終了する
        cerr << "can't load video" << endl;
        return -1;
    }
#else /*RELEASE*/
    frame.cap.open(0); // デフォルトカメラをオープン
    frame.start = std::chrono::system_clock::now();
    if (!frame.cap.isOpened()) {  // 成功したかどうかをチェック
        cerr << "can't open video!" << endl;
        return -1;
    }
#endif /*_DEBUG OR RELEASE*/
    while (true) {
        keisoku1 = std::chrono::system_clock::now();
        frame.cap >> frame.img; //USBカメラが得た動画の１フレームを格納    
        keisoku2 = std::chrono::system_clock::now();
        stringstream time1;
        time1 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku2 - keisoku1).count()) / 1000; // 小数点とか直したい
        cout << "time1: " << time1.str() << endl;
        // 元画像の縦横のサイズ(アノテーション部位を引き延ばすときのサイズ計算用)
        frame.frameWidth = frame.img.cols;
        frame.frameHeight = frame.img.rows;

        // 動画保存と画面表示の際の情報を取得
        frame.width = (int)frame.cap.get(cv::CAP_PROP_FRAME_WIDTH);	// フレーム横幅を取得
        frame.height = (int)frame.cap.get(cv::CAP_PROP_FRAME_HEIGHT);	// フレーム縦幅を取得
        frame.fps = frame.cap.get(cv::CAP_PROP_FPS);				// フレームレートを取得

        //----------------前処理-----------------------------------------------------------------------------------------------
        cv::Mat inputBlob = cv::dnn::blobFromImage(frame.img, 0.5, cv::Size(inWidth, inHeight), false, false);
        frame.net.setInput(inputBlob); // netにblobを渡す
        cv::Mat detection = frame.net.forward(); //4-dim:nchw
        // N: Number of detected objects as face
        // C: color channels
        // H: Height
        // W: Width
        //----------------------------------------------------------------ここから顔検知--------------------------------------------------------------------
        cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
        // detection.size[2]: 確信度
        // detection.size[3]: x1, y1, x2, y2・・・
        // フレームごとに新しくvectorを設定する
        int x1;
        int y1;
        int x2;
        int y2;

        int j = 0;
        keisoku3 = std::chrono::system_clock::now();
        for (int i = 0; i < detectionMat.rows; i++)
        {
            float confidence = detectionMat.at<float>(i, 2); // 検出した部位の信頼度の下限値
            // 顔と検出する
            if (confidence > confidenceThreshold)
            {
                int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frame.frameWidth);
                int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frame.frameHeight);
                int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frame.frameWidth);
                int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frame.frameHeight);
                // imgに赤枠をかける(赤枠表示・非表示を切り替えられるようにする) コマンドラインパーサーでしたい
                // フレームカウント，タイムスタンプの表示・非表示切り替え
    //-----------------------------------------検出した顔に赤枠をかける，検出率を表示(切り替えるようにする)--------------------------------------------------------
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
        time2 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku4 - keisoku3).count()) / 1000; // 小数点とか直したい
        cout << "time2: " << time2.str() << endl;
        // デバッグモードでサンプル動画の読み込み，リリースモードでcamera動画の読み込み
        // フレーム数の表示
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
        // プログラム起動時からの時間を表示
        auto end = std::chrono::system_clock::now();  // プログラム起動からの計測終了時間
        auto diff = end - frame.start; // 経過時間
        stringstream time;
        time << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::microseconds>(diff).count()) / 1000000; // 小数点とか直したい
        cv::putText(
            frame.img,
            "time:" + time.str() + " [s]",
            cv::Point(100, 15),
            cv::FONT_HERSHEY_SIMPLEX,
            0.5,
            cv::Scalar(0, 0, 255),
            1
        );
        // フルスクリーン表示する
        static const std::string kWinName = "privacy camera";
        cv::namedWindow(kWinName, cv::WINDOW_NORMAL);
        cv::setWindowProperty(kWinName, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
        cv::imshow(kWinName, frame.img);
        keisoku8 = std::chrono::system_clock::now();
        stringstream time4;
        time4 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku8 - keisoku7).count()) / 1000; // 小数点とか直したい
        cout << "time4: " << time4.str() << endl;
        keisoku9 = std::chrono::system_clock::now();
        frame.writer << frame.img;
        keisoku10 = std::chrono::system_clock::now();
        stringstream time5;
        time5 << fixed << setprecision(3) << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(keisoku10 - keisoku9).count()) / 1000; // 小数点とか直したい
        cout << "time5: " << time5.str() << endl;
        // qキーが押されたら画面を閉じる
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }
    // プログラムの終了
    return 0;

}

// 平滑化を行うメンバ関数　 円形にブラーするか，顔検出した範囲から顔マスク取得してブラーかけるか
template<class T>
void FRAME<T>::cvSmooth(cv::Mat& src, cv::Mat& dst, int mode) {
    cv::Mat src_roi1;
    cv::Mat src_roi2;
    /*cv::Mat roi;*/
    switch (mode) {
    case CV_GAUSSIAN:

        cv::GaussianBlur(src, src, cv::Size(0, 0), 10); // src:入力　src:出力
        /*cout << "GaussianBlur" << endl;*/
        src_roi1 = cv::Mat(src, cv::Rect(src.cols / 4, src.rows / 4, src.cols - src.cols / 2, src.rows - src.rows / 2)); // 左上の頂点から切り取る幅と高さ
        cv::GaussianBlur(src_roi1, src_roi1, cv::Size(0, 0), 10);
        //cout << "GaussianBlur" << endl;
        src_roi2 = cv::Mat(src, cv::Rect(src.cols / 3, src.rows / 3, src.cols / 3, src.rows / 3)); // 左上の頂点から切り取る幅と高さ
        cv::GaussianBlur(src_roi2, src_roi2, cv::Size(0, 0), 10);
        //cout << "GaussianBlur" << endl;
        break;
    case CV_BLUR:
        cv::blur(src, src, cv::Size(5, 5)); // src:入力　src:出力 mode:フィルタ
        src_roi1 = cv::Mat(src, cv::Rect(src.cols / 4, src.rows / 4, src.cols - src.cols / 2, src.rows - src.rows / 2)); // 左上の頂点から切り取る幅と高さ
        cv::blur(src_roi1, src_roi1, cv::Size(5, 5));
        src_roi2 = cv::Mat(src, cv::Rect(src.cols / 3, src.rows / 3, src.cols / 3, src.rows / 3)); // 左上の頂点から切り取る幅と高さ
        cv::blur(src_roi2, src_roi2, cv::Size(5, 5));
        break;
    case CV_MEDIAN:
        cv::medianBlur(src, src, 5); // src:入力　src:出力 mode:フィルタ
        src_roi1 = cv::Mat(src, cv::Rect(src.cols / 4, src.rows / 4, src.cols - src.cols / 2, src.rows - src.rows / 2)); // 左上の頂点から切り取る幅と高さ
        cv::medianBlur(src_roi1, src_roi1, 5);
        src_roi2 = cv::Mat(src, cv::Rect(src.cols / 3, src.rows / 3, src.cols / 3, src.rows / 3)); // 左上の頂点から切り取る幅と高さ
        cv::medianBlur(src_roi2, src_roi2, 5);
        break;
    }
}

#endif
