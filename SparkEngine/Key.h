#ifndef _KEY_H_
#define _KEY_H_

template <class T>
class Key {
public:
	T value;
	double time;

	Key() {}
	Key(T v, double t);
};

typedef Key<float3> float3Key;
typedef Key<Quat> quatKey;

#endif

template<class T>
inline Key<T>::Key(T v, double t)
{
	value = v;
	time = t;
}
