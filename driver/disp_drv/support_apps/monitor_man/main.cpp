/*
	모니터 체크 및 관리 프로그램(출력부 라이브러리 테스트용 겸)


  개발해야 하는 기능

  v1. 모니터 나열 및 ExtEscape 통신 시도
  2. 해상도 조절하기

  http://www.codeproject.com/Articles/15072/Programmatically-change-display-resolution

  위치도 제어할 수 있을걸로 보인다
  https://valelab.ucsf.edu/svn/micromanager2/branches/micromanager1.3/DeviceAdapters/GenericSLM/DisplayAdapters.cpp
	3. 가상모니터 프레임 버퍼 확인

*/

#include "vx_driver.h"
#include <vector>

using namespace std;

int main(int argc, char *argv)
{
	vector<vx_mon_state*> mon_lst;
	
	vx_state *vx;
	int max,i;

	vx = vx_init();

	max = vx_get_devices(vx);
	for(i=0;i<max;i++)
	{
		vx_mon_state *vmon = vx_get_desc(vx,i);

		if(vmon)	//가상 모니터임
			mon_lst.push_back(vmon);
	}
	
	printf("the number of detected virtual monitor %d\n",mon_lst.size());

	

	vx_release(vx);
	return 0;
}
