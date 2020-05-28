#include "CPrecomp.h"
#include "CApp.h"

/*------------------------------------------------------------*/
/*-----CREATED BY AGNARES-------------------------------------*/
/*-----27.05.2020---------------------------------------------*/
/*-----SHARE, EDIT, LEARN, DO WHATEVER YOU WANT WITH THIS-----*/
/*------------------------------------------------------------*/

#define BORDER_WIDTH	(const int)50
#define BORDER_HEIGHT	(const int)25

#define SCORE_ADDITION  (const int)50
#define TAIL_ADDITION	(const int)1

#define TOTAL_LIVES		(const int)3

using namespace std::chrono_literals;

TestApp::TestApp(int nWidth, int nHeight)
: 
	m_Screen(NULL),
	m_dwBytesWritten(NULL),
	m_nWidth(nWidth), 
	m_nHeight(nHeight), 
	m_bEnd(false)
{
	m_nPlayerX = 0;
	m_nPlayerY = 0;
	m_nFruitX = 0;
	m_nFruitY = 0;
	m_nScore = 0;
	m_nLives = TOTAL_LIVES;
	eState = eHit::COUNT;
	m_bCheck = true;
	m_bSnakeBody = false;

	m_Hearts = new wchar_t[m_nLives];
	m_Screen = new wchar_t[m_nWidth * m_nHeight];

	ClearBuffer();
}

TestApp::~TestApp()
{
	delete[] m_Hearts;
	delete[] m_Screen;
}

wchar_t* TestApp::GetHearts()
{
	for (unsigned int i = 0; i < m_nLives; ++i)
	{
		m_Hearts[i] = *L"\u2665";
		if (i == m_nLives - 1)
		{
			m_Hearts[i + 1] = *L"\0";
		}
	}
	return m_Hearts;
}

void TestApp::GenerateFruit()
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist_width(1, BORDER_WIDTH - 2);
	std::uniform_int_distribution<std::mt19937::result_type> dist_height(1, BORDER_HEIGHT - 2);
	m_nFruitX = dist_width(rng);
	m_nFruitY = dist_height(rng);
	for (unsigned int i = 0; i < vTail.size(); ++i)
	{
		const std::tuple<int, int>& reftail = vTail[i];
		if (m_nFruitX == std::get<0>(reftail) && m_nFruitY == std::get<1>(reftail))
			GenerateFruit();
	}
}

void TestApp::ClearBuffer()
{
	for (unsigned int i = 0; i < m_nWidth * m_nHeight; ++i)
	{
		m_Screen[i] = ' ';
	}
}

bool TestApp::m_bCheckFruitPlayer()
{
	if (m_nPlayerX == m_nFruitX && m_nPlayerY == m_nFruitY)
		return true;
	return false;
}

bool TestApp::m_bCheckIntersection()
{
	for (unsigned int i = 0; i < vTail.size(); ++i)
	{
		const std::tuple<int, int> &reftail = vTail[i];
		if (m_nPlayerX == std::get<0>(reftail) && m_nPlayerY == std::get<1>(reftail))
		{
			for (unsigned int j = 0; j < i; ++j)
			{
				vTail.pop_back();
				m_nScore -= SCORE_ADDITION / TAIL_ADDITION;
			}
			m_nPlayerX = BORDER_WIDTH / 2;
			m_nPlayerY = BORDER_HEIGHT / 2;
			m_nLives -= 1;
			return true;
		}
	}
	return false;
}

bool TestApp::m_bCheckFruit(const int& nWidth, const int& nHeight)
{
	if (nWidth == m_nFruitX && nHeight == m_nFruitY)
		return true;
	return false;
}

bool TestApp::m_bCheckBorder(const int& nWidth, const int& nHeight)
{
	if (nWidth < 1 || nHeight < 1 || nWidth > BORDER_WIDTH - 2 || nHeight > BORDER_HEIGHT - 2)
		return true;
	return false;
}

bool TestApp::m_bCheckPlayer(const int& nWidth, const int& nHeight)
{
	if (nWidth == m_nPlayerX && nHeight == m_nPlayerY) 
	{
		m_bSnakeBody = false;
		return true;
	}
	else
	{
		m_bSnakeBody = true;
		for (unsigned int i = 0; i < vTail.size(); ++i)
		{
			if (nWidth == std::get<0>(vTail[i]) && nHeight == std::get<1>(vTail[i]))
				return true;
		}
	}
	return false;
}

// checking direction for speed difference x and y
bool TestApp::m_bCheckDirection(const eHit& eDirection)
{
	return eDirection == eHit::UP || eDirection == eHit::DOWN;
}

// checking if key is registered and setting a state
void TestApp::CheckHit()
{
	// keyboard catch
	if (_kbhit())
	{
		switch (_getch())
		{
			case 'w':
				if (eState != eHit::DOWN)
					eState = eHit::UP;
				break;
			case 'a':
				if (eState != eHit::RIGHT)
					eState = eHit::LEFT;
				break;
			case 's':
				if (eState != eHit::UP)
					eState = eHit::DOWN;
				break;
			case 'd':
				if (eState != eHit::LEFT)
					eState = eHit::RIGHT;
				break;
			default:
				break;
		}
	}

	// setting direction true y false x
	bool bDirection = false;
	if (m_bCheckDirection(eState))
		bDirection = true;

	// checking if new timepoint should be created
	if (m_bCheck)
	{
		tp = std::chrono::system_clock::now();
		m_bCheck = false;
	}

	// slowing up the game speed and syncing direction
	if ((std::chrono::system_clock::now() - tp) > (bDirection ? 100ms : 60ms))
	{
		// snake body part
		static unsigned int segmentCounter;
		wsprintfW(&m_Screen[3 * m_nWidth + (BORDER_WIDTH + 1)], L"SEGMENT: %d", segmentCounter);
		for (unsigned int i = 0; i < vTail.size(); ++i)
		{
			if (segmentCounter == i)
			{
				std::tuple<int, int> &reftail = vTail[i];
				std::get<0>(reftail) = m_nPlayerX;
				std::get<1>(reftail) = m_nPlayerY;
			}
		}
		if (vTail.size() > 0)
		{
			segmentCounter++;
		}
		if (segmentCounter > vTail.size() - 1)
		{
			segmentCounter = 0;
		}

		// checking state for movement
		switch (eState)
		{
			case eHit::UP:
				m_nPlayerY--;
				break;
			case eHit::DOWN:
				m_nPlayerY++;
				break;
			case eHit::LEFT:
				m_nPlayerX--;
				break;
			case eHit::RIGHT:
				m_nPlayerX++;
				break;
			default:
				break;
		}

		// setting new timepoint
		m_bCheck = true;
	}
}

// displaying main game over screen
void TestApp::DisplayGameOver()
{
	ClearBuffer();
	wsprintfW(&m_Screen[11 * m_nWidth + 40], L"+-----------------------------------+");
	wsprintfW(&m_Screen[12 * m_nWidth + 40], L" GAMEOVER PRESS ENTER TO PLAY AGAIN!" );
	wsprintfW(&m_Screen[13 * m_nWidth + 40], L" YOUR SCORE: %d", m_nScore		     );
	wsprintfW(&m_Screen[14 * m_nWidth + 40], L"+-----------------------------------+");
}

// displaying chrono fps
void TestApp::DisplayFps()
{
	static auto tp_fps = std::chrono::system_clock::now();
	static int fps;
	static bool bFirtDisplayFps = false;

	if (!bFirtDisplayFps)
	{
		wsprintfW(&m_Screen[0 * m_nWidth + (BORDER_WIDTH + 1)], L"FPS: %d", 0);
		bFirtDisplayFps = true;
	}

	fps++;
	if ((std::chrono::system_clock::now() - tp_fps) > 1000ms)
	{
		tp_fps = std::chrono::system_clock::now();
		wsprintfW(&m_Screen[0 * m_nWidth + (BORDER_WIDTH + 1)], L"FPS: %d", fps);
		fps = 0;
	}
}

// displaying main frame containing debug info
void TestApp::DisplayMainFrame()
{
	wsprintfW(&m_Screen[1 * m_nWidth + (BORDER_WIDTH + 1)], L"SCORE: %d", m_nScore);
	wsprintfW(&m_Screen[2 * m_nWidth + (BORDER_WIDTH + 1)], L"POSX: %d POSY: %d", m_nPlayerX, m_nPlayerY);
	wsprintfW(&m_Screen[4 * m_nWidth + (BORDER_WIDTH + 1)], L"LIVES(%d): %s", m_nLives, GetHearts());
	wsprintfW(&m_Screen[5 * m_nWidth + (BORDER_WIDTH + 1)], L"=====SNAKE BY AGNARES======");
}

// default writing to a screen buffer called each loop
void TestApp::Write()
{
	m_bCheckIntersection();

	for (int i = 0; i < BORDER_WIDTH; ++i)
	{
		for (int j = 0; j < BORDER_HEIGHT; ++j)
		{
			if (m_bCheckBorder(i, j))
				m_Screen[j * m_nWidth + i] = '#';
			else if (m_bCheckPlayer(i, j))
				m_Screen[j * m_nWidth + i] = m_bSnakeBody ? 'o' : 'O';
			else if (m_bCheckFruit(i, j))
				m_Screen[j * m_nWidth + i] = 'F';
			else
				m_Screen[j * m_nWidth + i] = ' ';
		}
	}

	DisplayFps();
	DisplayMainFrame();

	if (m_bCheckFruitPlayer())
	{
		GenerateFruit();
		m_nScore += SCORE_ADDITION;
		for (int i = 0; i < TAIL_ADDITION; ++i)
		{
			vTail.push_back(std::make_tuple(0, 0));
		}
	}

	if (m_bCheckBorder(m_nPlayerX, m_nPlayerY))
	{
		m_nLives -= 1;
		m_nPlayerX = BORDER_WIDTH / 2;
		m_nPlayerY = BORDER_HEIGHT / 2;
	}

	if (m_nLives == 0)
	{
		m_bEnd = true;
		vTail.clear();
		DisplayGameOver();
	}

	WriteConsoleOutputCharacterW(GetHandle(), m_Screen, m_nWidth * m_nHeight, { 0, 0 }, &m_dwBytesWritten);
}

void TestApp::Run()
{
	while (true)
	{
		// game restart
		ClearBuffer();

		m_nPlayerX = BORDER_WIDTH / 2;
		m_nPlayerY = BORDER_HEIGHT / 2;
		eState = eHit::COUNT;
		m_bCheck = true;
		m_bSnakeBody = false;
		m_nScore = 0;
		m_nLives = TOTAL_LIVES;

		GenerateFruit();

		// idling for user input for a new run
		while (m_bEnd)
		{
			if (_kbhit())
			{
				if (_getch() == '\r')
					m_bEnd = false;
			}
		}

		// main game loop
		while (!m_bEnd)
		{
			CheckHit();
			Write();
		}
	}
}