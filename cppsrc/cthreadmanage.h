
class CThreadPool
{
}

class CJob
{
}

class CThreadManage
{
private:
	CThreadPool* mpool;//pointer to real threadpool
	int m_NumOfThread;//max thread number when bingfa created.
protected;

public:
	void SetParallelNum(int num);
	CThreadManage();
	CThreadManage(int num);
	virtual ~CThreadManage();

	void Run(CJob* job, void* jobdata);
	void TerminateAll(void);
};
