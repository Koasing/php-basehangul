# BaseHangul C implementation

BaseHangul은 한글을 사용한 바이너리 인코더입니다.

- KS X 1001 표준에 정의된 한글 문자 순서대로 1024 + 4 + 1문자를 사용하여 10비트를 한 글자(2바이트)로 변환합니다.
- 기본적으로 처음 1024개의 문자를 사용하되, Padding을 위하여 예외적으로 4 + 1 문자를 사용합니다.

- BaseHangul 규격에 대한 설명은 http://api.dcmys.jp/basehangul/ 를 참고하십시오.
- 웹 버전의 작동 시험은 http://api.dcmys.jp/%EB%B7%81%EC%96%B4%EB%B2%88%EC%97%AD%EA%B8%B0/ 에서 해볼 수 있습니다.

## 문법

- encode 와 decode 모두 입출력 버퍼와 버퍼길이를 입력받습니다.
- 출력 버퍼의 길이를 0으로 지정하는 경우 변환에 필요한 출력 버퍼 길이를 반환하고 종료합니다.
- 입출력 버퍼의 길이가 합당한 경우, 변환을 수행하고 실제 출력된 출력 버퍼 길이를 반환합니다.
- 실제 출력된 출력 버퍼 길이는 변환에 필요한 출력 버퍼 길이보다 작거나 같을 수 있습니다. 남는 공간은 0으로 채워지지만 구현 특징이며 사용하여서는 안 됩니다.
- 입력값이 올바르지 않은 경우 -1을, 내부 오류가 발생한 경우 -2를 반환합니다.

## 사용 예제

동봉된 test.cpp 를 참고하십시오.

## 라이센스

BaseHangul 규격은 Public Domain 으로 공개되어 있습니다.

BaseHangul C implementation은 The BSD 2-clause license로 공개되어 있습니다.
원저작자 표기를 포함하는 조건으로 누구나 자유롭게 수정, 개작, 재배포를 할 수 있습니다.
자세한 사항은 LICENSE 파일을 참고하십시오.
