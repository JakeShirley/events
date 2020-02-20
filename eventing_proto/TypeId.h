/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

// Provides a unique identifier at compile time to every template instance of typeid<Foo>:
//
//  Usage:
//
//     typeid_t typeId = mcpe::type_id(foo);
//     if (typeId == someOtherTypeId) { ... }
//

//  Works with RTTI off in standards compliant manner.  However, note that aggressive optimization
//  techniques can theoretically cause this not to work due to COMDAT folding.  In practice, however,
//  the complexity of the class housing the result of the template function generally prevents this.
//  For a stronger guarantee, turn on RTTI and use std::type_index.

template<typename Namespace>
class typeid_t {
public:

	template<typename N, typename T>
	friend typeid_t<N> type_id();

	typeid_t()
		: mID(0xffffu) {
	}

	typeid_t(const typeid_t& rhs)
		: mID(rhs.mID) {
	}

	typeid_t operator=(const typeid_t& rhs) {
		mID = rhs.mID;
		return *this;
	}

	bool operator==(const typeid_t& o) const {
		return mID == o.mID;
	}

	bool operator!=(const typeid_t& o) const {
		return mID != o.mID;
	}

	bool operator<(const typeid_t& o) const {
		return mID < o.mID;
	}

	bool operator<(size_t id) const {
		return mID < static_cast<uint16_t>(id);
	}

	uint16_t value() const {
		return mID;
	}
	static uint16_t count;

private:

	enum NewIDType {
		New
	};

	uint16_t mID;
	typeid_t(NewIDType)
		: mID(count++) {
	}

};

template<typename Namespace>
uint16_t typeid_t<Namespace>::count = 0;


template<typename Namespace, typename T>
typeid_t<Namespace> type_id() {
	static typeid_t<Namespace> id(typeid_t<Namespace>::New);
	return id;
}
