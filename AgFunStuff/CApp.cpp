#include "CPrecomp.h"
#include "CApp.h"

/*------------------------------------------------------------*/
/*-----CREATED BY AGNARES-------------------------------------*/
/*-----27.05.2020---------------------------------------------*/
/*-----SHARE, EDIT, LEARN, DO WHATEVER YOU WANT WITH THIS-----*/
/*------------------------------------------------------------*/

// basic defines
#define BORDER_WIDTH	(const int)80
#define BORDER_HEIGHT	(const int)25

#define SCORE_ADDITION  (const int)50
#define TAIL_ADDITION	(const int)1

#define TOTAL_LIVES		(const int)3
#define HEART_RANDOM_OF	(const int)6

using namespace std::chrono_literals;

// constructor
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
	m_nHeartX = 0;
	m_nHeartY = 0;
	m_nScore = 0;
	m_nLives = TOTAL_LIVES;
	eState = eHit::COUNT;
	m_bDirection = true;
	m_bCheck = true;
	m_bSnakeBody = false;
	m_bAvailableHeart = false;

	vHighscore.push_back(0);

	m_Hearts = new wchar_t[m_nLives];
	m_Screen = new wchar_t[m_nWidth * m_nHeight];

	ClearBuffer(0, m_nWidth * m_nHeight);
}

// destructor
TestApp::~TestApp()
{
	delete[] m_Hearts;
	delete[] m_Screen;
}

// returning wchar_t pointer with hearts
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

// creating random seed and generating fruit coordinates where tail is not present
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

// creating random seed and generating heart coordinates where tail is not present
void TestApp::GenerateHeart()
{
	if (m_nLives < TOTAL_LIVES)
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist_width(1, BORDER_WIDTH - 2);
		std::uniform_int_distribution<std::mt19937::result_type> dist_height(1, BORDER_HEIGHT - 2);
		std::uniform_int_distribution<std::mt19937::result_type> dist_availability(1, HEART_RANDOM_OF);
		m_nHeartX = dist_width(rng);
		m_nHeartY = dist_height(rng);
		dist_availability(rng) == unsigned int(true) ? (m_bAvailableHeart = true) : (m_bAvailableHeart = false);
		for (unsigned int i = 0; i < vTail.size(); ++i)
		{
			const std::tuple<int, int>& reftail = vTail[i];
			if (m_nHeartX == std::get<0>(reftail) && m_nHeartY == std::get<1>(reftail))
				GenerateHeart();
		}
	}
}

// clearing whole buffer length
void TestApp::ClearBuffer(const int& nFrom, const int& nTo)
{
	for (int i = nFrom; i < nTo; ++i)
	{
		m_Screen[i] = ' ';
	}
}

// setting color to output buffer
void TestApp::SetColor(const COORD& cCoords, const DWORD& dwLength, const WORD& wAttribute)
{
	FillConsoleOutputAttribute(GetHandle(), wAttribute, dwLength, cCoords, &m_dwAttributesWritten);
}

// checking if player head coordinates equal fruit coordinates
bool TestApp::m_bCheckFruitPlayer()
{
	if (m_nPlayerX == m_nFruitX && m_nPlayerY == m_nFruitY)
		return true;
	return false;
}

// checking if player head coordinates equal heart coordinates
bool TestApp::m_bCheckHeartPlayer()
{
	if ((m_nPlayerX == m_nHeartX && m_nPlayerY == m_nHeartY) && m_bAvailableHeart == true)
		return true;
	return false;
}

// looping through tail deque and checking if coordinates of player head and tail match or no
bool TestApp::m_bCheckIntersection()
{
	if (eState != eHit::COUNT)
	{
		for (unsigned int i = 0; i < vTail.size(); ++i)
		{
			if (m_nPlayerX == std::get<0>(vTail[i]) && m_nPlayerY == std::get<1>(vTail[i]))
			{
				for (unsigned int j = 0; j < i; ++j)
				{
					vTail.pop_back();
					m_nScore -= SCORE_ADDITION / TAIL_ADDITION;
				}
				m_nLives -= 1;
				return true;
			}
		}
	}
	return false;
}

// checking if fruit coordinates equal buffer width and height
bool TestApp::m_bCheckFruit(const int& nWidth, const int& nHeight)
{
	if (nWidth == m_nFruitX && nHeight == m_nFruitY)
	{
		SetColor({ (SHORT)m_nFruitX, (SHORT)m_nFruitY }, 1, 6);
		return true;
	}
	return false;
}

// checking if heart coordinates equal buffer width and height
bool TestApp::m_bCheckHeart(const int& nWidth, const int& nHeight)
{
	if (nWidth == m_nHeartX && nHeight == m_nHeartY)
	{
		if (m_bAvailableHeart)
		{
			SetColor({ (SHORT)m_nHeartX, (SHORT)m_nHeartY }, 1, 5);
		}
		return true;
	}
	return false;
}

// checking if player head has crossed the border line
bool TestApp::m_bCheckBorder(const int& nWidth, const int& nHeight)
{
	if (nWidth < 1 || nHeight < 1 || nWidth > BORDER_WIDTH - 2 || nHeight > BORDER_HEIGHT - 2)
		return true;
	return false;
}

// checking if player head coordinates equal buffer width and height same for snake tail
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
	// setting direction true y false x
	m_bDirection = false;
	if (m_bCheckDirection(eState))
	{
		m_bDirection = true;
	}

	// checking if new timepoint should be created
	if (m_bCheck)
	{
		tp = std::chrono::system_clock::now();
		m_bCheck = false;
	}

	// slowing up the game speed and syncing direction
	if ((std::chrono::system_clock::now() - tp) > (m_bDirection ? 60ms : 30ms))
	{
		// checking if snake cut his body
		m_bCheckIntersection();

		// snake body part
		static unsigned int segmentCounter;
		for (unsigned int i = 0; i < vTail.size(); ++i)
		{
			if (segmentCounter == i)
			{
				std::tuple<int, int>& reftail = vTail[i];
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
	ClearBuffer(0, m_nWidth * m_nHeight);
	wsprintfW(&m_Screen[11 * m_nWidth + 40], L"+-----------------------------------+");
	wsprintfW(&m_Screen[12 * m_nWidth + 40], L" GAMEOVER PRESS ENTER TO PLAY AGAIN!" );
	wsprintfW(&m_Screen[13 * m_nWidth + 40], L" YOUR SCORE: %d", m_nScore		     );
	wsprintfW(&m_Screen[14 * m_nWidth + 40], L"+-----------------------------------+");

	for (int i = 0; i < 4; ++i)
	{
		SetColor({ 40, 11 + (SHORT)i }, 38, 4);
	}

	vHighscore.push_back(m_nScore);
}

// displaying main frame containing debug info
void TestApp::DisplayMainFrame()
{
	SHORT nOffset = 0;
	SHORT nParamTextLength = 28;

	for (int i = 0; i < nOffset + 5; ++i)
	{
		ClearBuffer(i * m_nWidth + (BORDER_WIDTH + 1), i * m_nWidth + (BORDER_WIDTH + nParamTextLength));
	}

	wsprintfW(&m_Screen[(nOffset + 0) * m_nWidth + (BORDER_WIDTH + 1)], L"SCORE: %d", m_nScore);
	wsprintfW(&m_Screen[(nOffset + 1) * m_nWidth + (BORDER_WIDTH + 1)], L"HIGHSCORE: %d FROM %d", *std::max_element(vHighscore.begin(), vHighscore.end()), vHighscore.size() - 1);
	wsprintfW(&m_Screen[(nOffset + 2) * m_nWidth + (BORDER_WIDTH + 1)], L"LIVES: %s", GetHearts());
	wsprintfW(&m_Screen[(nOffset + 3) * m_nWidth + (BORDER_WIDTH + 1)], L"DIRECTION: %s", eState == eHit::COUNT ? L"[NONE]" : m_bDirection ? (eState == eHit::UP ? L"[UP]" : L"[DOWN]") : (eState == eHit::LEFT ? L"[LEFT]" : L"[RIGHT]"));
	wsprintfW(&m_Screen[(nOffset + 4) * m_nWidth + (BORDER_WIDTH + 1)], L"=====SNAKE BY AGNARES======");

	SetColor({ (BORDER_WIDTH + 1 ), nOffset + 0 }, 6, 9);						// blue score text
	SetColor({ (BORDER_WIDTH + 8 ), nOffset + 0 }, nParamTextLength, 11);		// light blue score
	SetColor({ (BORDER_WIDTH + 1 ), nOffset + 1 }, 10, 9);						// blue highscore text
	SetColor({ (BORDER_WIDTH + 11), nOffset + 1 }, nParamTextLength, 11);		// blue highscore
	SetColor({ (BORDER_WIDTH + 1 ), nOffset + 2 }, 6, 9);						// blue lives text
	SetColor({ (BORDER_WIDTH + 8 ), nOffset + 2 }, nParamTextLength, 5);		// purple lives
	SetColor({ (BORDER_WIDTH + 1 ), nOffset + 3 }, 10, 9);						// blue direction text
	SetColor({ (BORDER_WIDTH + 11), nOffset + 3 }, nParamTextLength, 2);		// green direction

	// agnares signature
	SetColor({ (BORDER_WIDTH + 1 ), nOffset + 4 }, nParamTextLength, 14);
}

// default writing to a screen buffer called each loop
void TestApp::Write()
{
	for (int i = 0; i < BORDER_WIDTH; ++i)
	{
		for (int j = 0; j < BORDER_HEIGHT; ++j)
		{
			if (m_bCheckBorder(i, j)) 
			{
				SetColor({ (SHORT)i, (SHORT)j }, 1, 4);
				m_Screen[j * m_nWidth + i] = '*';
			}
			else if (m_bCheckPlayer(i, j))
			{
				SetColor({ (SHORT)i, (SHORT)j }, 1, m_bSnakeBody ? 2 : 10);
				m_Screen[j * m_nWidth + i] = m_bSnakeBody ? 'o' : 'O';
			}
			else if (m_bCheckFruit(i, j))
			{
				m_Screen[j * m_nWidth + i] = 'F';
			}
			else if (m_bCheckHeart(i, j))
			{
				if (m_bAvailableHeart)
				{
					m_Screen[j * m_nWidth + i] = *L"\u2665";
				}
			}
			else
			{
				m_Screen[j * m_nWidth + i] = ' ';
			}
		}
	}

	DisplayMainFrame();

	if (m_bCheckFruitPlayer())
	{
		m_nScore += SCORE_ADDITION;
		SetColor({ (SHORT)m_nFruitX, (SHORT)m_nFruitY }, 1, 7);
		GenerateFruit();
		if (!m_bAvailableHeart)
		{
			GenerateHeart();
		}
		for (int i = 0; i < TAIL_ADDITION; ++i)
		{
			vTail.push_back(std::make_tuple(0, 0));
		}
	}

	if (m_bCheckHeartPlayer())
	{
		m_bAvailableHeart = false;
		if (m_nLives < TOTAL_LIVES)
		{
			m_nHeartX = 0;
			m_nHeartY = 0;
			m_nLives += 1;
			GenerateHeart();
		}
	}

	if (m_bCheckBorder(m_nPlayerX, m_nPlayerY))
	{
		eState = eHit::COUNT;
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
		ClearBuffer(0, m_nWidth * m_nHeight);

		m_nPlayerX = BORDER_WIDTH / 2;
		m_nPlayerY = BORDER_HEIGHT / 2;
		eState = eHit::COUNT;
		m_bDirection = true;
		m_bCheck = true;
		m_bSnakeBody = false;
		m_bAvailableHeart = false;
		m_nScore = 0;
		m_nLives = TOTAL_LIVES;

		GenerateFruit();
		GenerateHeart();

		// idling for user input for a new run
		while (m_bEnd)
		{
			if (_kbhit())
			{
				if (_getch() == '\r')
				{
					m_bEnd = false;
				}
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
