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
	DWORD m_dwAttributesWritten;

	unsigned int m_nWidth, m_nHeight;
	unsigned int m_nPlayerX, m_nPlayerY;
	unsigned int m_nFruitX, m_nFruitY;
	unsigned int m_nHeartX, m_nHeartY;
	unsigned int m_nScore;
	unsigned int m_nLives;

	bool m_bEnd;
	bool m_bDirection;
	bool m_bCheck;
	bool m_bSnakeBody;
	bool m_bAvailableHeart;

	std::deque<std::tuple<int, int>> vTail;
	std::deque<int> vHighscore;

	eHit eState;
	std::chrono::system_clock::time_point tp;

private:
	wchar_t* GetHearts();
	void GenerateFruit();
	void GenerateHeart();
	void ClearBuffer(const int& nFrom, const int& nTo);
	void SetColor(const COORD& cCoords, const DWORD& dwLength, const WORD& wAttribute);
	bool m_bCheckFruitPlayer();
	bool m_bCheckHeartPlayer();
	bool m_bCheckIntersection();
	bool m_bCheckFruit(const int& nWidth, const int& nHeight);
	bool m_bCheckHeart(const int& nWidth, const int& nHeight);
	bool m_bCheckBorder(const int& nWidth, const int& nHeight);
	bool m_bCheckPlayer(const int& nWidth, const int& nHeight);
	bool m_bCheckDirection(const eHit& eDirection);
	void CheckHit();
	void DisplayGameOver();
	void DisplayMainFrame();
	void Write();

public:
	void Run();

public:
	TestApp(int nWidth, int nHeight);
	~TestApp();
};