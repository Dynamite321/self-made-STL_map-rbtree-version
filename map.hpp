/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>

//Red-Black Tree Version

#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map
{
	friend class iteraotr;
	friend class const_iterator;
private:
	typedef pair<const Key, T> Value;
	struct node
	{
		Value data;
		node *left;
		node *right;
		node *parent;
		int color; //red:0, black:1
		bool is_end;
		node(const Value &v, node *l = NULL, node *r = NULL,node *p = NULL, int c = 0, bool b = false) :data(v),left(l), right(r), parent(p), color(c), is_end(b) {}
		~node() {}
	};
	node *root;
	node *header;
	size_t node_count;

	void init()
	{
		void *tmp = operator new(sizeof(node));
		header = reinterpret_cast<node*>(tmp);
		header->parent = root;
		header->left = header;
		header->right = header;
		header->is_end = true;
		header->color = 0;
	}

	void clear(node *n)
	{
		if (n == NULL)
		{
			return;
		}
		clear(n->left);
		clear(n->right);
		delete n;
		n = NULL;
	}

	node* minimum(node *x)
	{
		while (x->left != NULL)
			x = x->left;
		return x;
	}

	node* maximum(node* x)
	{
		while (x->right != NULL)
			x = x->right;
		return x;
	}

	void leftRotate(node* x)
	{
		node *y = x->right;
		x->right = y->left;
		if (y->left != NULL)
			y->left->parent = x;
		y->parent = x->parent;
		if (x->parent == header)
		{
			root = y;
			header->parent = y;
			root->parent = header;
		}
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->left = x;
		x->parent = y;
	}

	void rightRotate(node *y)
	{
		node *x = y->left;
		y->left = x->right;
		if (x->right != NULL)
			x->right->parent = y;
		x->parent = y->parent;
		if (y->parent == header)
		{
			root = x;
			header->parent = x;
			root->parent = header;
		}
		else if (y == y->parent->left)
			y->parent->left = x;
		else
			y->parent->right = x;
		x->right = y;
		y->parent = x;
	}

	void insert_rebalance(node *x)
	{
		x->color = 0;
		while (x != root && x->parent->color == 0)
		{
			if (x->parent == x->parent->parent->left)
			{
				node *y = x->parent->parent->right;
				if (y != NULL && y->color == 0)
				{
					x->parent->color = 1;
					y->color = 1;
					x->parent->parent->color = 0;
					x = x->parent->parent;
				}
				else
				{
					if (x == x->parent->right)
					{
						x = x->parent;
						leftRotate(x);
					}
					x->parent->color = 1;
					x->parent->parent->color = 0;
					rightRotate(x->parent->parent);
				}
			}
			else
			{
				node *y = x->parent->parent->left;
				if (y != NULL && y->color == 0)
				{
					x->parent->color = 1;
					y->color = 1;
					x->parent->parent->color = 0;
					x = x->parent->parent;
				}
				else
				{
					if (x == x->parent->left)
					{
						x = x->parent;
						rightRotate(x);
					}
					x->parent->color = 1;
					x->parent->parent->color = 0;
					leftRotate(x->parent->parent);
				}
			}
		}
		root->color = 1;
	}

	node* erase_rebalance(node* z)
	{
		node* y = z;
		node* x = NULL;
		node* x_parent = NULL;
		int tmp;
		if (y->left == NULL)
			x = y->right;
		else
			if (y->right == NULL)
				x = y->left;
			else
			{
				y = y->right;
				while (y->left != NULL)
					y = y->left;
				x = y->right;
			}
			if (y != z)
			{
				z->left->parent = y;
				y->left = z->left;
				if (y != z->right)
				{
					x_parent = y->parent;
					if (x) x->parent = y->parent;
					y->parent->left = x;
					y->right = z->right;
					z->right->parent = y;
				}
				else
					x_parent = y;
				if (root == z)
					root = y;
				else if (z->parent->left == z)
					z->parent->left = y;
				else
					z->parent->right = y;
				y->parent = z->parent;
				tmp = y->color;
				y->color = z->color;
				z->color = tmp;
				y = z;

			}
			else
			{
				x_parent = y->parent;
				if (x) x->parent = y->parent;
				if (root == z)
					root = x;
				else
					if (z->parent->left == z)
						z->parent->left = x;
					else
						z->parent->right = x;
				if (header->left == z)
					if (z->right == NULL)
						header->left = z->parent;

					else
						header->left = minimum(x);
				if (header->right == z)
					if (z->left == NULL)
						header->right = z->parent;

					else
						header->right = maximum(x);
			}
			if (y->color != 0)
			{
				while (x != root && (x == NULL || x->color == 1))
				{
					if (x == x_parent->left)
					{
						node* w = x_parent->right;
						if (w->color == 0)
						{
							w->color = 1;
							x_parent->color = 0;
							leftRotate(x_parent);
							w = x_parent->right;
						}
						if ((w->left == NULL ||
							w->left->color == 1) &&
							(w->right == NULL ||
								w->right->color == 1))
						{
							w->color = 0;
							x = x_parent;
							x_parent = x_parent->parent;
						}
						else
						{
							if (w->right == NULL || w->right->color == 1)
							{
								if (w->left != NULL)
									w->left->color = 1;
								w->color = 0;
								rightRotate(w);
								w = x_parent->right;
							}
							w->color = x_parent->color;
							x_parent->color = 1;
							if (w->right != NULL)
								w->right->color = 1;
							leftRotate(x_parent);
							break;
						}
					}
					else
					{
						node* w = x_parent->left;
						if (w->color == 0)
						{
							w->color = 1;
							x_parent->color = 0;
							rightRotate(x_parent);
							w = x_parent->left;
						}
						if ((w->right == NULL || w->right->color == 1) &&
							(w->left == NULL ||
								w->left->color == 1))
						{
							w->color = 0;
							x = x_parent;
							x_parent = x_parent->parent;
						}
						else
						{
							if (w->left == NULL || w->left->color == 1)
							{
								if (w->right != NULL)
									w->right->color = 1;
								w->color = 0;
								leftRotate(w);
								w = x_parent->left;
							}
							w->color = x_parent->color;
							x_parent->color = 1;
							if (w->left != NULL)
								w->left->color = 1;
							rightRotate(x_parent);
							break;
						}
					}
				}
				if (x != NULL)
					x->color = 1;
			}
			return y;
	}

	void copy_tree(node *n, node* &x, node *y)
	{
		if (n == NULL)
		{
			return;
		}
		x = new node(n->data);
		x->parent = y;
		x->is_end = n->is_end;
		x->color = n->color;
		copy_tree(n->left, x->left, x);
		copy_tree(n->right, x->right, x);
	}

public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
	class const_iterator;
	class iterator {
		friend class map;
		friend class const_iterator;
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		node *ptr;
		const map *container;
	public:
		iterator(node *p = NULL, const map *c = NULL) :ptr(p), container(c)
		{
			// TODO
		}
		iterator(const iterator &other)
		{
			// TODO
			ptr = other.ptr;
			container = other.container;
		}
		iterator(const const_iterator &other)
		{
			ptr = other.ptr;
			container = other.container;
		}
		~iterator() {}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		iterator &operator=(const iterator &rhs)
		{
			if (this == &rhs)
				return *this;
			ptr = rhs.ptr;
			container = rhs.container;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int)
		{
			if (ptr == container->header)
				throw invalid_iterator();
			else
			{
				iterator itr(*this);
				if (ptr->right != NULL)
				{
					ptr = ptr->right;
					while (ptr->left != NULL)
						ptr = ptr->left;
				}
				else
				{
					node *tmp = ptr->parent;
					while (ptr == tmp->right)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					if (ptr->right != tmp)
					{
						ptr = tmp;
					}
				}
				return itr;
			}
		}
		/**
		 * TODO ++iter
		 */
		iterator & operator++()
		{
			if (ptr == container->header)
				throw invalid_iterator();
			else
			{
				if (ptr->right != NULL)
				{
					ptr = ptr->right;
					while (ptr->left != NULL)
						ptr = ptr->left;
				}
				else
				{
					node *tmp = ptr->parent;
					while (ptr == tmp->right)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					if (ptr->right != tmp)
					{
						ptr = tmp;
					}
				}
				return *this;
			}
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int)
		{
			if (ptr == container->header->left)
				throw invalid_iterator();
			else
			{
				node *tmp;
				iterator itr(*this);
				if (ptr->is_end)
					ptr = ptr->right;
				else if (ptr->left != NULL)
				{
					tmp = ptr->left;
					while (tmp->right != NULL)
						tmp = tmp->right;
					ptr = tmp;
				}
				else
				{
					tmp = ptr->parent;
					while (ptr == tmp->left)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					ptr = tmp;
				}
				return itr;
			}
		}
		/**
		 * TODO --iter
		 */
		iterator & operator--()
		{
			if (ptr == container->header->left)
				throw invalid_iterator();
			else
			{
				node *tmp;
				if (ptr->is_end)
					ptr = ptr->right;
				else if (ptr->left != NULL)
				{
					tmp = ptr->left;
					while (tmp->right != NULL)
						tmp = tmp->right;
					ptr = tmp;
				}
				else
				{
					tmp = ptr->parent;
					while (ptr == tmp->left)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					ptr = tmp;
				}
				return *this;
			}
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const
		{
			return ptr->data;
		}
		bool operator==(const iterator &rhs) const
		{
			return (ptr == rhs.ptr);
		}
		bool operator==(const const_iterator &rhs) const
		{
			return (ptr == rhs.ptr);
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const
		{
			return (ptr != rhs.ptr);
		}
		bool operator!=(const const_iterator &rhs) const
		{
			return (ptr != rhs.ptr);
		}

		value_type* operator->() const noexcept
		{
			return &(ptr->data);
		}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		friend class map;
		friend class iterator;
	private:
		// data members.
		node *ptr;
		const map *container;
	public:
		const_iterator(node *p = NULL, const map *c = NULL) :ptr(p), container(c)
		{
			// TODO
		}
		const_iterator(const const_iterator &other)
		{
			// TODO
			ptr = other.ptr;
			container = other.container;
		}
		const_iterator(const iterator &other)
		{
			// TODO
			ptr = other.ptr;
			container = other.container;
		}
		~const_iterator() {}
		// And other methods in iterator.
		// And other methods in iterator.
		// And other methods in iterator.
		const_iterator &operator=(const const_iterator &rhs)
		{
			if (this == &rhs)
				return *this;
			ptr = rhs.ptr;
			container = rhs.container;
			return *this;
		}
		const_iterator operator++(int)
		{
			if (ptr == container->header)
				throw invalid_iterator();
			else
			{
				const_iterator itr(*this);
				if (ptr->right != NULL)
				{
					ptr = ptr->right;
					while (ptr->left != NULL)
						ptr = ptr->left;
				}
				else
				{
					node *tmp = ptr->parent;
					while (ptr == tmp->right)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					if (ptr->right != tmp)
					{
						ptr = tmp;
					}
				}
				return itr;
			}
		}
		const_iterator &operator++()
		{
			if (ptr == container->header)
				throw invalid_iterator();
			else
			{
				if (ptr->right != NULL)
				{
					ptr = ptr->right;
					while (ptr->left != NULL)
						ptr = ptr->left;
				}
				else
				{
					node *tmp = ptr->parent;
					while (ptr == tmp->right)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					if (ptr->right != tmp)
					{
						ptr = tmp;
					}
				}
				return *this;
			}
		}
		const_iterator operator--(int)
		{
			if (ptr == container->header->left)
				throw invalid_iterator();
			else
			{
				node *tmp;
				const_iterator itr(*this);
				if (ptr->is_end)
					ptr = ptr->right;
				else if (ptr->left != NULL)
				{
					tmp = ptr->left;
					while (tmp->right != NULL)
						tmp = tmp->right;
					ptr = tmp;
				}
				else
				{
					tmp = ptr->parent;
					while (ptr == tmp->left)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					ptr = tmp;
				}
				return itr;
			}
		}
		const_iterator operator--()
		{
			if (ptr == container->header->left)
				throw invalid_iterator();
			else
			{
				node *tmp;
				if (ptr->is_end)
					ptr = ptr->right;
				else if (ptr->left != NULL)
				{
					tmp = ptr->left;
					while (tmp->right != NULL)
						tmp = tmp->right;
					ptr = tmp;
				}
				else
				{
					tmp = ptr->parent;
					while (ptr == tmp->left)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					ptr = tmp;
				}
				return *this;
			}
		}

		const value_type & operator*() const
		{
			return ptr->data;
		}
		bool operator==(const iterator &rhs) const
		{
			return (ptr == rhs.ptr);
		}
		bool operator==(const const_iterator &rhs) const
		{
			return (ptr == rhs.ptr);
		}
		bool operator!=(const iterator &rhs) const
		{
			return (ptr != rhs.ptr);
		}
		bool operator!=(const const_iterator &rhs) const
		{
			return (ptr != rhs.ptr);
		}
		value_type* operator->() const noexcept
		{
			return &(ptr->data);
		}
	};
	/**
	 * TODO two constructors
	 */
	map()
	{
		root = NULL;
		init();
		node_count = 0;
	}
	map(const map &other)
	{
	    root = NULL;
		init();
		node_count = other.node_count;
		copy_tree(other.root, root, header);
		if (root != NULL)
		{
			node *tmp = root;
			while (tmp->left != NULL)
			{
				tmp = tmp->left;
			}
			header->left = tmp;
			node *tmp2 = root;
			while (tmp2->right != NULL)
			{
				tmp2 = tmp2->right;
			}
			header->right = tmp2;
		}
		header->parent = root;
	}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other)
	{
		if (this == &other)
			return *this;
		clear();
		node_count = other.node_count;
		copy_tree(other.root, root, header);
		if (root != NULL)
		{
			node *tmp = root;
			while (tmp->left != NULL)
			{
				tmp = tmp->left;
			}
			header->left = tmp;
			node *tmp2 = root;
			while (tmp2->right != NULL)
			{
				tmp2 = tmp2->right;
			}
			header->right = tmp2;
		}
		header->parent = root;
		return *this;
	}
	/**
	 * TODO Destructors
	 */
	~map()
	{
		clear();
		operator delete (header);
	}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key)
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
			throw index_out_of_bound();
		else
			return tmp->data.second;
	}
	const T & at(const Key &key) const
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
			throw index_out_of_bound();
		else
			return tmp->data.second;
	}
	/**
	 * TODO
	 * access specified element
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key)
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
		{
			return((insert(value_type(key, T())).first.ptr->data).second);
		}
		else
			return tmp->data.second;
	}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
			throw index_out_of_bound();
		else
			return tmp->data.second;
	}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin()
	{
		iterator itr(header->left, this);
		return itr;
	}
	const_iterator cbegin() const
	{
		const_iterator itr(header->left, this);
		return itr;
	}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end()
	{
		iterator itr(header, this);
		return itr;
	}
	const_iterator cend() const
	{
		const_iterator itr(header, this);
		return itr;
	}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const
	{
		return node_count == 0;
	}
	/**
	 * returns the number of elements.
	 */
	size_t size() const
	{
		return node_count;
	}
	/**
	 * clears the contents
	 */
	void clear()
	{
		clear(root);
		header->left = header;
		header->right = header;
		header->parent = root = NULL;
		node_count = 0;
	}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion),
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value)
	{
		node *y = header;
		node *x = root;
		bool comp = true;
		while (x != NULL)
		{
			y = x;
			comp = Compare()(value.first, x->data.first);
			x = comp ? x->left : x->right;
		}
		iterator j(y, this);
		if (comp)
		{
			if (j == begin())
				return pair<iterator, bool>(insert(value, x, y),true);
			else
				--j;
		}
		if (Compare()(j.ptr->data.first, value.first))
			return pair<iterator, bool>(insert(value, x, y), true);
		return pair<iterator, bool>(j, false);
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos)
	{
		if (pos.ptr == NULL || pos.ptr == header || pos.container != this)
			throw invalid_iterator();
		else
		{
			node* y = erase_rebalance(pos.ptr);
			delete y;
			--node_count;
		}
	}
	/**
	 * Returns the number of elements with key
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
			return 0;
		else
			return 1;
	}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key)
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
		{
			iterator itr(header, this);
			return itr;
		}
		else
		{
			iterator itr(tmp, this);
			return itr;
		}
	}
	const_iterator find(const Key &key) const
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
		{
			const_iterator itr(header, this);
			return itr;
		}
		else
		{
			const_iterator itr(tmp, this);
			return itr;
		}
	}
	private:
		iterator insert(const Value &value, node* x, node* y)
		{
			node *z;
			if (y == header || x != NULL || Compare()(value.first, y->data.first))
			{
				y->left = new node(value);
				z = y->left;
				if (y == header)
				{
					root = z;
					y->right = z;
					y->parent = z;
					root->parent = header;
				}
				else if (y == header->left)
					header->left = z;
			}
			else
			{
				y->right = new node(value);
				z = y->right;
				if (y == header->right)
					header->right = z;
			}
			z->parent = y;
			insert_rebalance(z);
			++node_count;
			return iterator(z, this);
		}

};

}

#endif
