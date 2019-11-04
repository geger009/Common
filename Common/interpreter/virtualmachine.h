//
// 2019-10-29, jjuiddong
// excute script virtual machine
//
#pragma once


namespace common
{
	namespace script
	{

		class cVirtualMachine
		{
		public:
			struct sRegister
			{
				uint idx; // excution command line index
				bool cmp; // compare output flag
				variant_t val[10]; // register
			};

			cVirtualMachine(const StrId &name);
			virtual ~cVirtualMachine();

			bool Init(const cIntermediateCode &code, iFunctionCallback *callback
				, void *arg = nullptr);
			bool Process(const float deltaSeconds);
			bool Run();
			bool Stop();
			bool PushEvent(const cEvent &evt);
			void Clear();


		protected:
			bool ProcessEvent();
			bool ExecuteInstruction(sRegister &reg);


		public:
			enum class eState { Stop, Run, Wait };
			eState m_state;
			StrId m_name;
			sRegister m_reg;
			cSymbolTable m_symbTable;
			cIntermediateCode m_code;
			queue<cEvent> m_events;
			iFunctionCallback *m_callback;
			void *m_callbackArgPtr;
		};

	}
}