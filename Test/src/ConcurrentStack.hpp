#ifndef LIST_H
#define LIST_H


#include <atomic>
#include <memory>
#include <concepts>
#include <optional>
#include <type_traits>

template<typename T>
concept Comparable = std::equality_comparable<T>;

template<Comparable T>
class ConcurrentStack {
	struct Node { 
		Node( T data, std::shared_ptr<Node> next ) noexcept( std::is_nothrow_move_constructible_v<T> )
			: data{ std::move( data ) }
			, next{ std::move( next ) }
		{ }

		T data; 
		std::shared_ptr<Node> next; 
	};

public:
	auto find( T value ) const -> std::optional<std::shared_ptr<T>>;
	auto front( ) const -> std::optional<std::shared_ptr<T>>;
	auto push_front( T value ) -> void;
	auto pop_front( ) ->std::optional<std::shared_ptr<T>>;
private:
	std::atomic<std::shared_ptr<Node>> head_;	
};

template<Comparable T>
auto ConcurrentStack<T>::find( T value ) const -> std::optional<std::shared_ptr<T>> {
	auto node{ head_.load( ) };
	while ( node && node->data != value ) {
		node = node->next;
	}
	if ( node ) {
		return std::shared_ptr<T>{ node, &node->data };
	}
	return std::nullopt;
}

template<Comparable T>
auto ConcurrentStack<T>::push_front( T value ) -> void {
	auto node{ std::make_shared<Node>( 
		std::move( value ), 
		head_.load( std::memory_order::relaxed ) 
	) };

	while ( !head_.compare_exchange_weak( node->next, node ) );
}

template<Comparable T>
auto ConcurrentStack<T>::front( ) const -> std::optional<std::shared_ptr<T>> {
	if ( auto node{ head_.load( ) }; node ) {
		return std::shared_ptr<T>{ node, &node->data };
	}
	return std::nullopt;
}

template<Comparable T>
auto ConcurrentStack<T>::pop_front( ) -> std::optional<std::shared_ptr<T>> {
	if ( auto node{ head_.load( ) }; node ) {
		while ( !head_.compare_exchange_weak( node, node->next ) );
		return std::shared_ptr<T>{ node, &node->data };
	}	
	return std::nullopt;
}


#endif // !LIST_H

