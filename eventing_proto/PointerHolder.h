#pragma once

struct IPointerHolder {
	virtual ~IPointerHolder() = default;
};

template<typename ClassType>
struct PointerHolder : public IPointerHolder {
	PointerHolder(ClassType* _owner) : owner(_owner) {}
	virtual ~PointerHolder() = default;
	ClassType* owner;
};
