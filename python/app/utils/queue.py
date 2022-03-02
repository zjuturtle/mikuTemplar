class StackWithMaxMin:
    def __init__(self):
        self.__stack = []
        self.__min_stack = []
        self.__max_stack = []

    def push(self, item):
        if self.is_empty():
            self.__min_stack.append(item)
            self.__max_stack.append(item)
        else:
            self.__min_stack.append(min(self.__min_stack[-1], item))
            self.__max_stack.append(max(self.__max_stack[-1], item))
        self.__stack.append(item)

    def pop(self):
        if self.is_empty():
            raise ValueError('Stack is empty')
        self.__min_stack.pop()
        self.__max_stack.pop()
        return self.__stack.pop()

    def size(self) -> int:
        return len(self.__stack)

    def is_empty(self) -> bool:
        return len(self.__stack) == 0

    def max(self):
        if self.is_empty():
            raise ValueError('Stack is empty')
        return self.__max_stack[-1]

    def min(self):
        if self.is_empty():
            raise ValueError('Stack is empty')
        return self.__min_stack[-1]

class QueueWithMaxMin:
    def __init_(self):
        self.__s1 = StackWithMaxMin()
        self.__s2 = StackWithMaxMin()

    def push(self, item):
        self.__s1.push(item)

    def pop(self):
        while not self.__s1.is_empty():
            self.__s2.push(self.__s1.pop())
        return self.__s2.pop()

    def is_empty(self):
        return self.__s1.is_empty() and self.__s2.is_empty()

    def size(self):
        return self.__s1.size() + self.__s2.size()

    def min(self):
        return min(self.__s1, self.__s2)

    def max(self):
        return max(self.__s1, self.__s2)

