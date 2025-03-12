# BlurSW
### 프로젝트 구조
- Custom_DLL/ : 직접 구현한 블러링 기능을 포함한 DLL
- OpenCV_DLL/ : OpenCV를 활용한 블러링 기능을 포함한 DLL
- ImageObjectLib/ : 이미지 처리 관련 라이브러리
- MainApp/ : 실행 가능한 메인 애플리케이션
- include/nlohmann/ : JSON 파싱을 위한 라이브러리 포함
  

### 빌드 및 실행 방법
1. Visual Studio에서 BlurSW.sln 솔루션 파일을 엽니다.
2. Release 또는 Debug 모드에서 빌드합니다.
3. MainApp을 실행하여 블러링 기능을 테스트합니다.

### 요구 사항
Visual Studio 2017 이상
C++17 이상 지원
OpenCV 설치 필요

### config 파일 설명
- image_path: Blur 함수 돌릴 이미지 저장된 폴더 위치
- kernel_size: Blur시 사용할 커널 사이즈
- result_path: Blur후 결과 이미지 저장될 위치

### 실행시 유의사항
- 같은 폴더내의 config 파일 읽음 다른 위치로 이동 X
- 같은 폴더내의 image_blur.log 에 log 기록
- 같은 폴더내의 TestReport.txt 에 테스트 결과 저장