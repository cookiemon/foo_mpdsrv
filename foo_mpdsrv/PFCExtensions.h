#ifndef PFCEXTENSIONS_H
#define PFCEXTENSIONS_H

namespace foo_mpdsrv
{
	class PfcHash
	{
	public:
		size_t operator()(const pfc::string8& _Keyval) const
		{	// hash _Keyval to size_t value by pseudorandomizing transform
			size_t _Val = 2166136261U;
			size_t _First = 0;
			size_t _Last = _Keyval.length();
			size_t _Stride = 1 + _Last / 10;

			for(; _First < _Last; _First += _Stride)
				_Val = 16777619U * _Val ^ (size_t)_Keyval[_First];
			return (_Val);
		}
	};
}

#endif