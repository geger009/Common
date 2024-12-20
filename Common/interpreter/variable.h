//
// 2021-02-21, jjuiddong
// variable
//	- symboltable variable
//		- primitive variable : variant_t 
//		- array 
//
// 2021-05-20
//	- add map type
//	- add enum symboltype
//
#pragma once


namespace common
{
	namespace script
	{
		class cSymbolTable;

		struct sVariable
		{
			sVariable();
			sVariable(const sVariable &rhs);
			~sVariable();

			bool IsReference() const;
			bool IsArray() const;
			bool IsMap() const;

			variant_t& GetArrayElement(const uint index);
			bool SetArrayElement(const uint index, const variant_t &v);
			bool PushArrayElement(const variant_t &v);
			int FindArrayElement(const variant_t& v);
			variant_t& PopArrayElement();
			bool ReserveArray(const uint size);
			uint GetArraySize() const;
			
			variant_t& GetMapValue(cSymbolTable &symbolTable, const string &key);
			bool SetMapValue(const string &key, const variant_t &v);
			bool HasMapValue(const string &key);
			uint GetMapSize() const;
			const string& GetMapValueTypeStr();
			const string& GetArrayElementTypeStr();

			sVariable& ShallowCopy(const sVariable& rhs);
			sVariable& operator=(const sVariable &rhs);
			void Clear();
			void ClearArray();
			void ClearArrayMemory();
			void ClearMap();
			void ClearMapMemory();


			int id; // unique id
			string type; // type string, Bool, Int, Float, String, Array, Map
			string subTypeStr; // array element, map value type string
			eSymbolType typeValues[4]; // maximum: map<string, vector<primitive type>>
			int flags; // 0x00:input, 0x02:output, 0x10:sync
			variant_t var; // value

			// array type
			uint arSize; // array size
			uint arCapacity; // array capacity
			variant_t *ar; // array

			// map type
			map<string, variant_t> *m;
		};

	}
}
