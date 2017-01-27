#ifndef __SUBSONGDLG_H
#define __SUBSONGDLG_H

//#include <sidplay/sidplay2.h>
#include "threadsidplayer.h"

class CSubSongDlg 
{
private:
	HWND m_hWnd;
	CThreadSidPlayer *m_player;
protected:
	void NextSubSong();
	void PrevSubSong();
public:
	CSubSongDlg(CThreadSidPlayer* player, HWND hWnd) {m_hWnd = hWnd; m_player = player; }
	~CSubSongDlg();
	int SubSongDlgWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Hide();
	void Show();
	void RefreshWindowTitle();
};

extern CSubSongDlg *subSongDlg;
int CALLBACK SubSongDlgWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


#endif //__SUBSONGDLG_H