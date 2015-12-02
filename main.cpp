#include <afx.h>
#include <iostream>
#include "P:\cppformat\format.h"

using namespace std;

#if _MSC_VER >= 1700
#define J_HAS_EMPLACE_BACK
#define J_HAS_RVALUE_REFERENCES
#endif

#include <vector>
namespace fmt
{
namespace internal
{

template<typename Char, typename Derived>
class ValueCollector
{
public:
	ValueCollector():types(0) {}

	template<class T>
	Derived& operator<<(const T& P)
	{
		size_t curValIx = getNbVal();
		Arg a = MakeValue<Char>(P);
		a.type = static_cast<internal::Arg::Type>(MakeValue<Char>::type(P));
		if (curValIx < ArgList::MAX_PACKED_ARGS)
		{
			types |= ULongLong(a.type & 0xf) << curValIx * 4;
			values[curValIx] = a;
		}
		if (curValIx == ArgList::MAX_PACKED_ARGS - 1)
		{
			//Convert to args
			args.reserve(ArgList::MAX_PACKED_ARGS);
			for (size_t i = 0; i < ArgList::MAX_PACKED_ARGS; ++i)
			{
				Arg arg;
				memcpy(&arg,&values[i],sizeof(values[i]));
				arg.type = Arg::Type((types & (0xfll << i * 4)) >> i * 4);
				args.push_back(arg);
			}
		}
		if (curValIx >= ArgList::MAX_PACKED_ARGS)
			args.push_back(a);
		return static_cast<Derived&>(*this);
	}

	ArgList argList()
	{
		if (!args.empty())
		{
#ifdef J_HAS_EMPLACE_BACK
			args.emplace_back(); //Empty value
#else
			args.push_back(Arg()); //Empty value
#endif
			return ArgList(types, &args[0]);
		}
		return ArgList(types, values);
	}

protected:
	size_t getNbVal() const
	{
		for (size_t i = 0; i < ArgList::MAX_PACKED_ARGS; ++i)
			if (!(types & (0xfll << i * 4)))
				return i;
		return args.size();
	}

	Value values[ArgList::MAX_PACKED_ARGS];
	ULongLong types;
	vector<Arg> args;
};

}

template<class Char>
class OldFormatter : public internal::ValueCollector<Char, OldFormatter<Char> >
{
public:
	OldFormatter(CStringRef P_Format):m_Format(P_Format), m_bFormatted(false){}
#ifdef J_HAS_RVALUE_REFERENCES
	OldFormatter(OldFormatter&& P):m_Writer(P.m_Writer), m_bFormatted(P.m_bFormatted), m_Format(P_Format){}
#endif

	~OldFormatter()
	{
	}

	const Char* c_str()
	{
		if (!m_bFormatted)
			m_Writer.write(m_Format, argList());
		m_bFormatted = true;
		return m_Writer.c_str();
	}

protected:
	BasicMemoryWriter<Char> m_Writer;
	bool m_bFormatted;
	CStringRef m_Format;
};

template <class Char>
OldFormatter<Char> OldFormat(const Char* P_Format)
{
	return OldFormatter<Char>(P_Format);
}

template <class Char>
const Char* c_str(OldFormatter<Char>& P)
{
	return P.c_str();
}

template <class Char>
const basic_string<Char> str(OldFormatter<Char>& P)
{
	return P.c_str();
}

} //namespace fmt::internal

int main(int argc, char* argv[])
{
	cout << "hello world!" << endl;
//	cout << fmt::format("Hello {} {}","world!", _MSC_VER) << endl;

/*	{
		fmt::internal::ValueCollector<char> W_Coll;
		W_Coll << "world!" << _MSC_VER;

		cout << fmt::format("Hello {} {}", W_Coll.argList()) << endl;
	}
*/
	char* bla = "bla";
	int blo = 3;
	// MAX_PACKED_ARGS - 1 args
	{
		cout << c_str(fmt::OldFormat("Hello {} {} {} {} - {} {} {} {} - {} {} {} {} - {} {} {}")
				<< "world!" << _MSC_VER
				<< bla << blo << bla << blo << bla << blo << bla << blo << bla << blo << string(bla) << blo << CStringA(bla))
			<< endl;
	}

	// MAX_PACKED_ARGS args
	{
		cout << c_str(fmt::OldFormat("Hello {} {} {} {} - {} {} {} {} - {} {} {} {} - {} {} {} {}")
				<< "world!" << _MSC_VER
				<< bla << blo << bla << blo << bla << blo << bla << blo << bla << blo << string(bla) << blo << CStringA(bla) << blo)
			<< endl;
	}

	// MAX_PACKED_ARGS + 1 args
	{
		cout << c_str(fmt::OldFormat("Hello {} {} {} {} - {} {} {} {} - {} {} {} {} - {} {} {} {} - {}")
				<< "world!" << _MSC_VER
				<< bla << blo << bla << blo << bla << blo << bla << blo << bla << blo << string(bla) << blo << CStringA(bla) << blo << bla)
			<< endl;
	}

	char c;
	cin >> c;
	return 0;
}
