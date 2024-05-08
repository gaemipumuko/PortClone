// BinaryTool.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "Binary_Defines.h"
#include "Convert_Manager.h"

#include "GameInstance.h"

int main()
{
    _int iReady = { };

    CConvert_Manager* pConvertMgr = CConvert_Manager::Create();

    cout << endl << "                               === ALERT ===" << endl;
    cout << "              Anim 파일은 ./BinaryTool/Convert/Anim/     폴더 안에 넣어주세요" << endl;
    cout << "           NonAnim 파일은 ./BinaryTool/Convert/NonAnim/  폴더 안에 넣어주세요" << endl;
    cout << "            SPAnim 파일은 ./BinaryTool/Convert/SPAnim/   폴더 안에 넣어주세요" << endl;

    cout << endl << "                               === ALERT ===" << endl;
    cout << "                          진행하기 위해서는 Enter를 눌러주세요" << endl;
    cin.get();

    if (FAILED(pConvertMgr->Convert_Anim(L"../Convert/Anim/*.*")))
        cout << "Failed to Convert Anim Model" << endl;
    else
        cout << "Successed to Convert Anim Model" << endl;

    if (FAILED(pConvertMgr->Convert_NonAnim(L"../Convert/NonAnim/*.*")))
        cout << "Failed to Convert NonAnim Model" << endl;
    else
        cout << "Successed to Convert NonAnim Model" << endl;

    if (FAILED(pConvertMgr->Convert_Anim_SPToonShader(L"../Convert/SPAnim/*.*")))
        cout << "Failed to Convert SPAnim Model" << endl;
    else
        cout << "Successed to Convert SPAnim Model" << endl;

    Safe_Release(pConvertMgr);
}