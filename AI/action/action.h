//
// Action ����
// �ൿ�� ó���ϴ� Ŭ������. cAction Ŭ������ �߻�Ŭ������ �ƹ� �ൿ��
// ���� �ʴ´�. ��ü���� �ൿ�� �� Ŭ������ ��ӹ޾Ƽ� ó���Ѵ�.
// Action Ʈ���� �����ϱ� ���� �������̽� ��
// �ൿ�� ���۰� ��, ����, �޼����� ó���ϴ� �������̽��� ���ǵȴ�.
//
// �ൿ�� ó���ϱ� ���� ���� �⺻���� ��ɰ� �������̽��� �����Ѵ�.
//
// 2016-05-14
//		- template class �� ����
//
#pragma once

#include "../aidef.h"


namespace ai
{
	using namespace common;

	template <class T>
	class cAction
	{
	public:
		cAction(
			T *agent = NULL,
			const StrId &name = "action",
			const StrId &animationName = "",
			const eActionType::Enum type = eActionType::NONE);
		virtual ~cAction();

		virtual bool Traverse(const float deltaSeconds);
		virtual bool ActionThrough(const float deltaSeconds);
		virtual bool ActionExecute(const float deltaSeconds);
		virtual bool StartAction();
		virtual bool MessageProccess(const sMsg &msg);
		virtual void Clear();

		void PushAction(cAction *action);
		void PushFrontAction(cAction *action);
		void PopAction();
		void RemoveAction(cAction *action);
		void RemoveAction(const StrId &actionName);
		void ClearChildAction();
		cAction* FindAction(const StrId &actionName, bool isFindChild=true);
		cAction* FindAction(const eActionType::Enum type, bool isFindChild=true);
		bool IsCurrentAction(const eActionType::Enum type);
		bool IsAction(const eActionType::Enum type);
		cAction* GetLeafAction();
		virtual int GetSubState() const;
		void SetActionState(const eActionState::Enum state);
		bool operator== (const cAction &rhs);


	protected:
		void NextAction();
		cAction* GetNextAction();


	public:
		eActionState::Enum m_state;
		eActionType::Enum m_type;
		StrId m_name;
		StrId m_animationName;
		T *m_agent;
		vector<cAction<T>*> m_children;
		cAction<T> *m_current;
	};


	//----------------------------------------------------------------
	// Implements

	template<class T>
	cAction<T>::cAction(
		T *agent //= NULL
		, const StrId &name //= "action"
		, const StrId &animationName //= ""
		, const eActionType::Enum type //= eActionType::NONE
	)
		: m_agent(agent)
		, m_name(name)
		, m_animationName(animationName)
		, m_type(type)
		, m_state(eActionState::WAIT)
		, m_current(NULL)
	{
	}

	template<class T>
	cAction<T>::~cAction()
	{
		Clear();
	}


	// Action�� ���� ���̶�� true�� �����ϰ�, 
	// �ƹ� �۾��� ���� �ʴ´ٸ� false �� �����Ѵ�.
	template<class T>
	bool cAction<T>::Traverse(const float deltaSeconds)
	{
		if (m_state != eActionState::RUN)
			if (!StartAction())
				NextAction();

		m_state = eActionState::RUN;

		if (m_current)
		{
			if (!ActionThrough(deltaSeconds))
				return true;

			if (!m_current->Traverse(deltaSeconds))
				NextAction();
		}
		else
		{
			if (!ActionExecute(deltaSeconds))
				return false;
		}

		return true;
	}


	// false�� �����ϸ� child�� Move()�� ȣ������ �ʴ´�. 
	// �Ļ��� Ŭ�������� �����Ѵ�.
	template<class T>
	bool cAction<T>::ActionThrough(const float deltaSeconds)
	{
		return true;
	}

	// Leaf��� �϶��� ȣ��ȴ�.
	// �ൿ�� ó���� �� ���⿡ �ڵ��� �ϸ�ȴ�.
	// false �� �����ϸ� �ൿ�� ����Ǿ��ٴ� ���̴�.
	// �Ļ��� Ŭ�������� �����Ѵ�.
	template<class T>
	bool cAction<T>::ActionExecute(const float deltaSeconds)
	{
		return true;
	}


	// �ൿ�� ó�� ���۵� �� ȣ��ȴ�.
	// �Ļ��� Ŭ�������� �����Ѵ�.
	// false�� �����ϸ�, �׼��� �����Ѵ�.
	template<class T>
	bool cAction<T>::StartAction()
	{
		return true;
	}


	// ���� ���� �ڽ� Action�� �޼����� ������.
	// �������̵��ؼ� �����Ѵ�.
	template<class T>
	bool cAction<T>::MessageProccess(const sMsg &msg)
	{
		if (m_current)
			m_current->MessageProccess(msg);
		return true;
	}


	template<class T>
	void cAction<T>::Clear()
	{
		m_state = eActionState::STOP;
		ClearChildAction();
	}


	// Action�� ���ÿ� �߰��Ѵ�.
	template<class T>
	void cAction<T>::PushAction(cAction *action)
	{
		action->m_agent = m_agent;
		m_children.push_back(action);

		// currentAction �缳���Ѵ�. currentAction�� ���� �ֱ� ���̾�� �Ѵ�.
		m_current = action;
	}


	// Action�� ���� Top�� �߰��Ѵ�.
	template<class T>
	void cAction<T>::PushFrontAction(cAction *action)
	{
		action->m_agent = m_agent;
		m_children.push_back(action);
		rotateright(m_children);

		if (m_children.size() == 1)
		{
			// Action�� �ϳ��ۿ� ���ٸ�, currentAction���� �����Ѵ�.
			m_current = action;
		}
	}


	// Action�� �����ϰ�, �� Action �� current�� �Ҵ��Ѵ�.
	template<class T>
	void cAction<T>::PopAction()
	{
		if (m_current)
		{
			delete m_current;
		}
		m_current = GetNextAction();
	}


	// ���ڷ� �Ѿ�� action�� �޸𸮱��� �����Ѵ�.
	template<class T>
	void cAction<T>::RemoveAction(cAction *action)
	{
		for (u_int i = m_children.size() - 1; i >= 0; --i)
		{
			if (*action == *m_children[i])
			{
				delete m_children[i];
				rotatepopvector(m_children, i);
			}
		}

		// currentAction �缳���Ѵ�.
		m_current = m_children.empty() ? NULL : m_children.back();
	}


	template<class T>
	void cAction<T>::RemoveAction(const StrId &actionName)
	{
		for (u_int i = m_children.size() - 1; i >= 0; --i)
		{
			if (actionName == m_children[i]->m_name)
			{
				delete m_children[i];
				rotatepopvector(m_children, i);
			}
		}

		// currentAction �缳���Ѵ�.
		m_current = m_children.empty() ? NULL : m_children.back();
	}


	// �ڽ����� �߰��� Action�� �����Ѵ�.
	template<class T>
	void cAction<T>::ClearChildAction()
	{
		for (auto action : m_children)
		{
			delete action;
		}
		m_children.clear();
		m_current = NULL;
	}


	template<class T>
	cAction<T>* cAction<T>::FindAction(const StrId &actionName, bool isFindChild //= true
	)
	{
		for (u_int i = 0; i < m_children.size(); ++i)
		{
			if (actionName == m_children[i]->m_name)
				return m_children[i];
		}

		if (isFindChild)
		{
			for (u_int i = 0; i < m_children.size(); ++i)
			{
				if (cAction *action = m_children[i]->FindAction(actionName, isFindChild))
					return action;
			}
		}

		return NULL;
	}


	template<class T>
	cAction<T>* cAction<T>::FindAction(const eActionType::Enum type, bool isFindChild //= true
	)
	{
		for (u_int i = 0; i < m_children.size(); ++i)
		{
			if (m_children[i]->IsCurrentAction(type))
				return m_children[i];
		}

		if (isFindChild)
		{
			for (u_int i = 0; i < m_children.size(); ++i)
			{
				if (cAction *action = m_children[i]->FindAction(type, isFindChild))
					return action;
			}
		}

		return NULL;
	}


	template<class T>
	bool cAction<T>::IsCurrentAction(const eActionType::Enum type)
	{
		return m_type == type;
	}


	// Action Ÿ���� ���Ѵ�. this �� current �߿� 
	// ���� �� Ÿ���� �ִٸ� true �� �����Ѵ�.
	template<class T>
	bool cAction<T>::IsAction(const eActionType::Enum type)
	{
		if (IsCurrentAction(type))
			return true;
		if (m_current)
			return m_current->IsAction(type);
		return false;
	}


	// Leaf ��带 �����Ѵ�.
	template<class T>
	cAction<T>* cAction<T>::GetLeafAction()
	{
		cAction *action = m_current;
		if (!action)
			return this;
		return action->GetLeafAction();
	}


	template<class T>
	int cAction<T>::GetSubState() const
	{
		return 0;
	}


	template<class T>
	void cAction<T>::SetActionState(const eActionState::Enum state)
	{
		m_state = state;
	}


	template<class T>
	bool cAction<T>::operator== (const cAction &rhs)
	{
		return m_name == rhs.m_name;
	}


	// ���� �׼����� �Ѿ��.
	template<class T>
	void cAction<T>::NextAction()
	{
		cAction *prevAction = m_current;
		m_current = GetNextAction();
		SAFE_DELETE(prevAction);
	}


	// ���� �׼��� �����Ѵ�.
	template<class T>
	cAction<T>* cAction<T>::GetNextAction()
	{
		if (m_children.empty())
			return NULL;
		m_children.pop_back();
		if (m_children.empty())
			return NULL;
		return m_children.back();
	}

}
