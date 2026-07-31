#ifndef SPEEDCIRCULARLIST_H
#define SPEEDCIRCULARLIST_H

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

constexpr int s_prime = 31;

inline int log2(int x) {
	int ans = 0;
	while (x != 1) {
		x /= 2;
		++ans;
	}
	return ans;
}

template<typename T, int b_prime>
class SpeedCircularLinkedList {
public:
    struct Node {
	    std::map<std::string, T> kv_map;
        int bound = 0;
        Node* next = nullptr;
        Node** fast_search_list = nullptr;

	    Node(int bound, int fast_search_list_size) : bound(bound) {
	    	if (fast_search_list_size > 0) {
	    		fast_search_list = new Node*[fast_search_list_size]{};
	    	}
	    }

	    ~Node() {
	    	delete[] fast_search_list;
	    }
    };

private:
	Node* head = nullptr;
	int fast_search_list_size = 0;
	int list_size = 0;

	static int GetHashCode(std::string str) {
		long long ans = 0;
		for (auto& ch : str) {
			ans = (ans * s_prime + ch) % b_prime;
		}
		return static_cast<int>((ans + b_prime) % b_prime);
	}

	void BuildFastSearchList() {
		if (!head) {
			return;
		}

		std::vector<Node*> nodes;
		nodes.reserve(list_size);
		Node* current = head;
		for (int index = 0; index < list_size; ++index) {
			nodes.push_back(current);
			current = current->next;
		}

		for (int index = list_size - 1; index >= 0; --index) {
			Node* node = nodes[index];
			if (fast_search_list_size > 0) {
				node->fast_search_list[0] = (index + 1 < list_size) ? nodes[index + 1] : nullptr;
				for (int jump = 1; jump < fast_search_list_size; ++jump) {
					Node* previous = node->fast_search_list[jump - 1];
					node->fast_search_list[jump] = previous ? previous->fast_search_list[jump - 1] : nullptr;
				}
			}
		}
	}

	Node* LocateNode(int code) const {
		if (!head) {
			return nullptr;
		}
		Node* current = head;
		if (code <= current->bound) return current;
		for (int jump = fast_search_list_size - 1; jump >= 0; --jump) {
			while (current->fast_search_list && current->fast_search_list[jump] && current->fast_search_list[jump]->bound < code) {
				current = current->fast_search_list[jump];
			}
		}
		return current->next;
	}

public:

    explicit SpeedCircularLinkedList(std::vector<int> node_bounds) {
		list_size = static_cast<int>(node_bounds.size());
		fast_search_list_size = 1;
		while ((1 << fast_search_list_size) < list_size) {
			++fast_search_list_size;
		}

		std::vector<Node*> nodes;
		nodes.reserve(list_size);
		for (int bound : node_bounds) {
			nodes.push_back(new Node(bound, fast_search_list_size));
		}

		for (int index = 0; index < list_size; ++index) {
			nodes[index]->next = nodes[(index + 1) % list_size];
		}
		head = list_size > 0 ? nodes[0] : nullptr;

		BuildFastSearchList();
    }

    ~SpeedCircularLinkedList() {
		if (!head) {
			return;
		}
		Node* current = head;
		for (int index = 0; index < list_size; ++index) {
			Node* next_node = current->next;
			delete current;
			current = next_node;
		}
    }

    void put(std::string str, T value) {
	    int code = GetHashCode(str);
	    if (!head) {
	    	return;
	    }
	    Node* target = LocateNode(code);
	    if (target) {
	    	target->kv_map[str] = value;
	    }
    }

	T get(std::string str) {
    	int code = GetHashCode(str);
	    if (!head) {
	    	return T();
	    }
	    Node* target = LocateNode(code);
	    if (!target) {
	    	return T();
	    }
	    auto it = target->kv_map.find(str);
	    if (it == target->kv_map.end()) {
	    	return T();
	    }
	    return it->second;
    }

    void print() {
		if (!head) {
			return;
		}
		Node* current = head;
		for (int index = 0; index < list_size; ++index) {
			std::cout << "[Node] Bound = " << current->bound
			          << ", kv_map_size = " << current->kv_map.size() << '\n';
			current = current->next;
		}
    }

    int size() const {
		return list_size;
    }
};

#endif //SPEEDCIRCULARLIST_H
