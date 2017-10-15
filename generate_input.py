#!/usr/bin/env python3
from random import randint


def main():
    name = open('dictionary/female-names.txt', 'r')
    city = open('dictionary/cities.txt', 'r')
    bad_in = open('bad_input.txt', 'w')
    match_in = open('match_input.txt', 'w')
    bad = []
    match = []

    for line in name:
        bad.append(line[:len(line)-1])

    for line in city:
        match.append(line[:len(line)-1])

    num = int(input())

    for i in range(num):
        bad_in.write('f\n')
        bad_in.write(bad[randint(0, len(bad)-1)]+'\n')

    for i in range(num):
        data = bad[randint(0, len(bad)-1)]
        bad_in.write('s\n')
        bad_in.write(data[:randint(1, len(data))]+'\n')

    for i in range(num):
        bad_in.write('d\n')
        bad_in.write(bad[randint(0, len(bad)-1)]+'\n')

    bad_in.write('e\nq\n')

    for i in range(num):
        match_in.write('f\n')
        match_in.write(match[randint(0, len(match)-1)]+'\n')

    for i in range(num):
        data = match[randint(0, len(match)-1)]
        match_in.write('s\n')
        match_in.write(data[:randint(1, len(data))]+'\n')

    for i in range(num):
        match_in.write('d\n')
        match_in.write(match[randint(0, len(match)-1)]+'\n')

    match_in.write('e\nq\n')


if __name__ == '__main__':
    main()
