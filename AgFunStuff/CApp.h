#pragma once
#include "CBase.h"
class TestApp : public CBase
{
private:
	enum class eHit
	{
		UP = 1, 
		DOWN, 
		LEFT, 
		RIGHT,
		COUNT
	};

private:
	wchar_t* m_Hearts;
	wchar_t* m_Screen;
	DWORD m_dwBytesWritten;
	unsigned int m_nWidth, m_nHeight;
	unsigned int m_nPlayerX, m_nPlayerY;
	unsigned int m_nFruitX, m_nFruitY;
	bool m_bEnd;
	bool m_bCheck;
	unsigned int m_nScore;
	unsigned int m_nLives;
	std::deque<std::tuple<int, int>> vTail;
	bool m_bSnakeBody;
	eHit eState;
	std::chrono::system_clock::time_point tp;

private:
	wchar_t* GetHearts();
	void GenerateFruit();
	void ClearBuffer();
	bool m_bCheckFruitPlayer();
	bool m_bCheckIntersection();
	bool m_bCheckFruit(const int& nWidth, const int& nHeight);
	bool m_bCheckBorder(const int& nWidth, const int& nHeight);
	bool m_bCheckPlayer(const int& nWidth, const int& nHeight);
	bool m_bCheckDirection(const eHit& eDirection);
	void CheckHit();
	void DisplayGameOver();
	void DisplayFps();
	void DisplayMainFrame();
	void Write();

public:
	void Run();

public:
	TestApp(int nWidth, int nHeight);
	~TestApp();
};