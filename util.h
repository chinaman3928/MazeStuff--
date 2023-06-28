#pragma once

#include "Random.h"


#include <vector>
#include <algorithm>
#include <functional>


class Coord
{
public:
    int i;
    int j;

    Coord(int i, int j)
        : i{ i }, j{ j } {}

    bool operator==(const Coord& other) const
    {
        return i == other.i && j == other.j;
    }

    Coord operator+(const Coord& other) const
    {
        return Coord(i + other.i, j + other.j);
    }

    void operator+=(const Coord& other)
    {
        i += other.i;
        j += other.j;
    }

    bool operator<(const Coord& other)
    {
        return (i < other.i) || (i == other.i && j < other.j);
    }
};


class GraphSets
{
public:
    GraphSets(int height, int width)
        : height{ height }, width{ width }
    {
        items.reserve(height * width);
        for (int i = 0; i != height * width; ++i)
        {
            items.emplace_back(i);
        }
    }

    bool same(Coord x, Coord y)
    {
        return findRoot(x.i * width + x.j) == findRoot(y.i * width + y.j);
    }

    void merge(Coord x, Coord y)
    {
        GraphSetItem& x_root_item = items[findRoot(x.i * width + x.j)];
        GraphSetItem& y_root_item = items[findRoot(y.i * width + y.j)];

        if      (x_root_item.rank > y_root_item.rank) y_root_item.parent = x_root_item.parent;
        else if (y_root_item.rank > x_root_item.rank) x_root_item.parent = y_root_item.parent;
        y_root_item.parent = x_root_item.parent;
        ++x_root_item.rank;
    }

private:
    struct GraphSetItem
    {
        int parent;
        int rank;
        GraphSetItem(int parent)
            : parent{parent}, rank{0}
        {
            //empty
        }
    };

    int height;
    int width;
    std::vector<GraphSetItem> items;

    int findRoot(int item)
    {
        int parent = items[item].parent;
        if (parent == item) return item;
        int root = findRoot(items[parent].parent);
        items[item].parent = root;
        return root;
    }
};

class LinkedList
{
private:

     struct LinkedListNode
    {
    public:
        int val;
        LinkedListNode* next;

        LinkedListNode(int val)
            : val{ val }, next{ nullptr }
        {
            //empty
        }
    };

public:
    LinkedListNode* first;
    LinkedListNode* last;
    int size;

    LinkedList(int firstArg)
        : first{ new LinkedListNode(firstArg) }, last{ first }, size{ 1 }
    {
        //empty
    }

    void add(int val)
    {
        last = last->next = new LinkedListNode(val);
        ++size;
    }

    void extend(const LinkedList& other)
    {
        last->next = other.first;
        last = other.last;
        size += other.size;
    }

    ~LinkedList()
    {
        while (first != nullptr)
        {
            auto firstprime = first->next;
            delete first;
            first = firstprime;
        }
    }
};

template <typename T>
class HashTableVector
{
private:
    std::vector<std::vector<T>> outer;
    std::vector<T> vec;
    std::function<unsigned(const T&)> hash;
    size_t numElems;

public:
    HashTableVector(int size, std::function<unsigned(const T&)> hash)
        : outer(size), hash{ hash }, numElems{ 0 }
    {
        //empty
    }

    void add(T elem)
    {
        std::vector<T>& inner = outer[hash(elem) % outer.size()];
        if (std::find(inner.begin(), inner.end(), elem) == inner.end())
        {
            inner.emplace_back(elem);
            vec.emplace_back(elem);
            ++numElems;
        }
    }

    T removeRandomElement(Random& r)
    {
        const int vec_i = r.uniformInt(0, vec.size() - 1);
        const T elem = vec[vec_i];
        vec[vec_i] = vec.back();
        vec.pop_back();
        std::vector<T>& inner = outer[hash(elem) % outer.size()];
        inner[std::find(inner.begin(), inner.end(), elem) - inner.begin()] = inner.back();
        inner.pop_back();
        --numElems;
        return elem;
    }

    size_t size() const { return numElems; }
};