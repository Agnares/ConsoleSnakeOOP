#include "CPrecomp.h"
#include "CBase.h"

/*------------------------------------------------------------*/
/*-----CREATED BY AGNARES-------------------------------------*/
/*-----27.05.2020---------------------------------------------*/
/*-----SHARE, EDIT, LEARN, DO WHATEVER YOU WANT WITH THIS-----*/
/*------------------------------------------------------------*/

CBase::CBase()
	:m_Handle(NULL)
{
	CreateBuffer();
}

CBase::~CBase()
{
	delete m_Handle;
}

void CBase::CreateBuffer()
{
	m_Handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); 
	SetBuffer();
}

void CBase::SetBuffer()
{
	SetConsoleActiveScreenBuffer(m_Handle);
}