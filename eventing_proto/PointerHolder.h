#pragma once

struct IPtrHolderBase {
	virtual ~IPtrHolderBase() = default;
};

template<typename ClassType>
struct PtrHolder : public IPtrHolderBase {
	PtrHolder(ClassType* _owner) : owner(_owner) {}
	virtual ~PtrHolder() = default;
	ClassType* owner;
};
