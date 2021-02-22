//
// 2019-10-29, jjuiddong
// interpreter symboltable
//
// 2020-02-21
//	- add array variable type
//	- reference https://codemuri.tistory.com/5
//
#pragma once


namespace common
{
	namespace script
	{

		class cSymbolTable
		{
		public:
			enum class eType { None, Enum, };

			// symbol information
			struct sEnum {
				string name;
				int value;
			};
			struct sSymbol {
				eType type;
				string name;
				vector<sEnum> enums;
			};

			//// variable information
			//struct sVar 
			//{
			//	int id; // unique id
			//	string type; // special type string, Bool, Int, Float, String, Array~
			//	variant_t var; // value

			//	// array type
			//	int subType0; // array element type, map key type (variant_t::VARTYPE)
			//	int subType1; // map value type (variant_t::VARTYPE)
			//	uint arSize; // array size
			//	uint arCapacity; // array capacity
			//	variant_t *ar; // array

			//	sVar();
			//	sVar(const sVar &rhs);
			//	~sVar();
			//	sVar& operator=(const sVar &rhs);
			//	void Clear();
			//};


			cSymbolTable();
			cSymbolTable(const cSymbolTable &rhs);
			virtual ~cSymbolTable();

			// variable
			template <class T>
			bool Set(const string &scopeName, const string &symbolName, const T &var
				, const string &typeStr="");
			template <class T, size_t N>
			bool Set(const string &scopeName, const string &symbolName, const T(&var)[N]
				, const string &typeStr = "");
			bool Set(const string &scopeName, const string &symbolName, const variant_t &var
				, const string &typeStr = "");
			bool SetArray(const string &scopeName, const string &symbolName
				, const variant_t &var, const string &typeStr = "");

			template <class T>
			T Get(const string &scopeName, const string &symbolName);
			bool Get(const string &scopeName, const string &symbolName, OUT variant_t &out);
			
			sVariable* FindVarInfo(const string &scopeName, const string &symbolName);
			bool IsExist(const string &scopeName, const string &symbolName);
			bool RemoveVar(const string &scopeName, const string &symbolName);
			
			// symbol
			bool AddSymbol(const sSymbol &type);
			bool RemoveSymbol(const string &typeName);
			sSymbol* FindSymbol(const string &typeName);

			bool CopySymbols(const cSymbolTable &rhs);
			void Clear();
			cSymbolTable& operator=(const cSymbolTable &rhs);

			static int GenID();
			static string MakeScopeName(const string &name, const int id);
			static std::pair<string, int> ParseScopeName(const string &scopeName);


		public:
			map<string, map<string, sVariable>> m_vars; // key: scopeName
												   // value: varName, value
			map<string, sSymbol*> m_symbols; // key: symbol name
			map<int, std::pair<string,string>> m_varMap; // key: variable id, value: scopeName, varName
														 // for array type

			static std::atomic_int s_genId;
		};


		//---------------------------------------------------------------------------------------
		// template function
		template <class T>
		inline bool cSymbolTable::Set(const string &scopeName, const string &symbolName
			, const T &var, const string &typeStr //= ""
			)
		{
			variant_t v((T)var);
			const bool r = Set(scopeName, symbolName, v, typeStr);
			common::clearvariant(v);
			return r;
		}

		// template specialization (string)
		template <>
		inline bool cSymbolTable::Set<string>(const string &scopeName, const string &symbolName
			, const string &var, const string &typeStr //= ""
			)
		{
			variant_t v;
			v.vt = VT_BSTR;
			v.bstrVal = ::SysAllocString(common::str2wstr(var).c_str());

			// to avoid bstr memory move bug
			common::clearvariant(m_vars[scopeName][symbolName].var);
			m_vars[scopeName][symbolName].var = v;
			m_vars[scopeName][symbolName].type = typeStr;
			return true;
		}

		// initialize array type
		template <class T, size_t N>
		inline bool cSymbolTable::Set(const string &scopeName, const string &symbolName
			, const T(&var)[N], const string &typeStr //= ""
			)
		{			
			sVariable arVar;
			arVar.type = "Array";
			arVar.arSize = N;
			arVar.ar = (N > 0) ? new variant_t[N] : nullptr;
			for (uint i = 0; i < N; ++i)
				arVar.ar[i] = var[i];
			m_vars[scopeName][symbolName] = arVar;
			return true;
		}

		template <class T>
		inline T cSymbolTable::Get(const string &scopeName, const string &symbolName) 
		{
			variant_t var;
			if (!Get(scopeName, symbolName, var))
				return variant_t((T)0);
			return (T)var;
		}

		// template specialization (string)
		template <>
		inline string cSymbolTable::Get<string>(const string &scopeName, const string &symbolName) 
		{
			variant_t var;
			if (!Get(scopeName, symbolName, var))
				return "";
			return WStr128(var.bstrVal).str().c_str();
		}

	}
}
