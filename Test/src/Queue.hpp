#ifndef QUEUE_H
#define QUEUE_H

#include <vector>
#include <tuple>
#include <atomic>
#include <memory>
#include <stdexcept>

namespace sync::mpsc {

	namespace detail {
		struct Pointers {
			int head;			
			int tail;
		};

		template<typename T>
		struct State { 
			State( std::size_t capacity )
				: queue( capacity )
			{ }
			std::vector<T> queue;
			Pointers pointers{ .head = 0, .tail = -1 };
		};
	}

	template<typename T>
	class Producer;

	template<typename T>
	class Consumer;

	template<typename T>
	using Pair = std::pair<Producer<T>, Consumer<T>>;

	template<typename T>
	auto create_channel( int capacity ) -> Pair<T> {
		if ( capacity < 1 ) {
			throw std::invalid_argument{ "Capacity must greater than zero" };
		}

		const auto size{ static_cast<std::size_t>( capacity ) };
		auto consumer{ std::make_shared<detail::State<T>>( size ) };
		auto producer{ consumer };

		return std::pair{
			Producer<T>{ std::move( producer ) },
			Consumer<T>{ std::move( consumer ) }
		};
	}

	template<typename T>
	class Producer { 
		friend auto create_channel<>( int capacity ) -> Pair<T>;
	public:
		auto enqueue( T item ) -> void;
	private:
		explicit Producer( std::shared_ptr<detail::State<T>> state ) noexcept;
		std::shared_ptr<detail::State<T>> state_;
	};

	template<typename T>
	auto Producer<T>::enqueue( T item ) -> void {

	}

	template<typename T>
	Producer<T>::Producer( std::shared_ptr<detail::State<T>> state ) noexcept 
		: state_{ std::move( state ) }
	{ }

	template<typename T>
	class Consumer { 
		friend auto create_channel<>( int capacity ) -> Pair<T>;
	public:		
		Consumer( Consumer&& ) = default;
		Consumer& operator=( Consumer&& ) = default;

		Consumer( const Consumer& ) = delete;
		Consumer& operator=( const Consumer& ) = delete;

		auto dequeue( ) -> T;
	private:
		explicit Consumer( std::shared_ptr<detail::State<T>> state ) noexcept;
		std::shared_ptr<detail::State<T>> state_;
	};

	template<typename T>
	inline Consumer<T>::Consumer( std::shared_ptr<detail::State<T>> state ) noexcept 
		: state_{ std::move( state ) }
	{ }
}

#endif 
