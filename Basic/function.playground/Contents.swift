import Foundation

func function1() {
    print("feeee")
}

function1()

func function2(value: Int) {
    let newvalue = value + 2
}

function2(value: 20)

func function3(value: Int) -> Int {
    return value + 2
}

print(function3(value: 20))

func function4(value1: Int, value2: Int) -> Int {
    value1 + value2
}

let val2 = function4(value1: 1, value2: 30)

func function5(_ yee: Int, _ Yee: Int) -> Int {
    yee - Yee
}

let val3 = function5(5, 6)


@discardableResult
func function6(_ yee: Int, _ Yee: Int) -> Int {
    yee - Yee
}

func basefunc(with value: Int) -> Int {
    func func1(value: Int) -> Int {
        value + 1
    }
    return func1(value: value + 4)
}
basefunc(with: 20)

func function7(firstName: String = "yee", lastName: String = "eet") -> String {
    "\(firstName) \(lastName)"
}

function7()
function7(firstName: "notpe")
function7(lastName: "notpe")

