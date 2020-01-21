#include <cassert>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <array>
#include <set>
#include <map>

using Octet = unsigned char;
using Address = std::array<Octet, 4>;
using Pool = std::multiset<Address>;
using PoolUnique = std::set<Address>;
using RIndex = std::map<Octet, PoolUnique>;

template <typename Iter, typename D>
Address split(Iter begin, Iter end, D delimiter)
{
    Address address;

    size_t index = 0;
    while (begin != end)
    {
        Iter pos = std::find(begin, end, delimiter);
        int chunk = std::stoi(std::string(begin, pos));
        assert(chunk >= 0 && chunk <= 255);
        address[index++] = static_cast<Octet>(chunk);
        begin = (pos == end) ? end : std::next(pos);
    }

    assert(index == 4);
    return address;
}

std::ostream& operator<< (std::ostream &out, const Address& address)
{
    bool first = true;
    for (const auto& octet : address)
    {
        if (!first)
        {
            out << '.';
        }
        out << static_cast<unsigned int>(octet);
        first = false;
    }
    return out;
}

template <typename Container>
void print(std::ostream &out, const Container& pool)
{
    bool first = true;
    for (auto pos = pool.rbegin(); pos != pool.rend(); ++pos)
    {
        if (!first)
        {
            out << '\n';
        }
        out << *(pos);
        first = false;
    }
}

std::ostream& operator<< (std::ostream &out, const Pool& pool)
{
    print(out, pool);
    return out;
}

std::ostream& operator<< (std::ostream &out, const PoolUnique& pool)
{
    print(out, pool);
    return out;
}

template <bool any = false, typename ...Args>
PoolUnique filter(RIndex rIndex, Octet first, Args... args)
{
    Address target{first, static_cast<Octet>(args)...};

    PoolUnique pool = rIndex.at(target[0]);
    if (any)
    {
        return pool;
    }

    PoolUnique result;

    std::copy_if(pool.begin(), pool.end(),  std::inserter(result, result.end()), [&target](const Address& address)
    {
        size_t size = target.size();
        for (size_t i = 0; i < size; ++i)
        {
            if(address[i] != target[i])
            {
                return false;
            }
        }
        return true;
    });

    return result;
}

int main()
{
    try
    {
        Pool pool;
        RIndex rIndex;

        for(std::string line; std::getline(std::cin, line);)
        {
            //TODO @i.lukin remove on done
            if (line.empty())
            {
                break;
            }

            auto pos = std::find(line.begin(), line.end(), '\t');
            Address address = split(line.begin(), pos, '.');

            for (size_t i = 0; i < 4; i++)
            {
                Octet octet = address[i];
                rIndex[octet].insert(address);
            }

            pool.insert(address);
        }

        std::cout << pool << std::endl;
        // 222.173.235.246
        // 222.130.177.64
        // 222.82.198.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        std::cout << filter(rIndex, 1) << std::endl;
        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        std::cout << filter(rIndex, 46, 70) << std::endl;
        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76

        std::cout << filter<true>(rIndex, 46) << std::endl;
        // 186.204.34.46
        // 186.46.222.194
        // 185.46.87.231
        // 185.46.86.132
        // 185.46.86.131
        // 185.46.86.131
        // 185.46.86.22
        // 185.46.85.204
        // 185.46.85.78
        // 68.46.218.208
        // 46.251.197.23
        // 46.223.254.56
        // 46.223.254.56
        // 46.182.19.219
        // 46.161.63.66
        // 46.161.61.51
        // 46.161.60.92
        // 46.161.60.35
        // 46.161.58.202
        // 46.161.56.241
        // 46.161.56.203
        // 46.161.56.174
        // 46.161.56.106
        // 46.161.56.106
        // 46.101.163.119
        // 46.101.127.145
        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76
        // 46.55.46.98
        // 46.49.43.85
        // 39.46.86.85
        // 5.189.203.46
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}