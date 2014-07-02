#include <windows.h>

typedef struct vx_state vx_state;
typedef struct vx_mon_state vx_mon_state;

vx_state *vx_init();			//드라이버 초기화
void vx_release(vx_state *vx);		//드라이버 해제
int vx_get_devices(vx_state *vx);	//목록을 새로 얻어옴
void vx_set_devices_position();	//모니터들의 배치 및 해상도를 변경함
vx_mon_state *vx_get_desc(vx_state *vs,int idx);		//해당 드라이버의 디스크립터를 얻어옴
BOOL vx_mon_isUpdate(vx_mon_state *mon);	//해당 모니터가 업데이트 되었나 ?
void vx_mon_tokenUpdate(vx_mon_state *mon);
PCHAR vx_mon_getFramebuffer(vx_mon_state *mon);	//framebuffer의 포인터를 얻어옴
POINT vx_mon_getResolution(vx_mon_state *mon);	//해상도 정보를 얻어옴
void vx_mon_changeResolution(vx_mon_state *mon,POINT res);	//해당 모니터의 해상도를 변경