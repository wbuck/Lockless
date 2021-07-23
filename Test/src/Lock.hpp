#ifndef LOCK_H
#define LOCK_H

#include <windows.h>
#include <memory>

namespace sync {
	using Cs = CRITICAL_SECTION;
	class CriticalSection {
	public:
		CriticalSection( ) noexcept;
		~CriticalSection( );

		CriticalSection( const CriticalSection& ) = delete;
		CriticalSection& operator=( const CriticalSection& ) = delete;

		auto lock( ) noexcept -> void;
		auto unlock( ) noexcept -> void;
	private:
		Cs cs_{ };
	};

	inline CriticalSection::CriticalSection( ) noexcept {
		InitializeCriticalSection( &cs_ );
	}

	inline CriticalSection::~CriticalSection( ) {
		DeleteCriticalSection( &cs_ );
	}

	inline auto CriticalSection::lock( ) noexcept -> void {
		EnterCriticalSection( &cs_ );
	}

	inline auto CriticalSection::unlock( ) noexcept -> void {
		LeaveCriticalSection( &cs_ );
	}

}
#endif // !LOCK_H
