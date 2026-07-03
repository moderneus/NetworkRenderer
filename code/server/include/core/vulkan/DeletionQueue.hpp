#pragma once

#include <functional>
#include <deque>

namespace core::vk {

class DeletionQueue {
	private:
		std::deque<std::function<void()>> deletionQueue;

	public:
		void Push(std::function<void()>);
		void CleanUp();
};

}
