#pragma once
class CBase
{
private:
	void CreateBuffer();
	void SetBuffer();

	HANDLE m_Handle;

protected:
	inline HANDLE const GetHandle() const { return m_Handle; }

public:
	CBase();
	~CBase();
};
