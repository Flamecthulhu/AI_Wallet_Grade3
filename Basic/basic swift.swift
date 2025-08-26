let x = 20 //Constant
var y = 20

let a: Int = 5
let b: Double = 3.14
let c: String = "Hello"
let d: Bool = true

func add(x: Int,y: Int) -> Int {
    return x + y
}
add(x: 3, y: 5)

func sub(_ x: Int, _ y: Int) -> Int { x - y }
sub(10, 3)

let n = 10
if n > 5 {
    print("大於 5")
} else {
    print("小於等於 5")
}

switch n {
case 1:
    print("一")
case 2, 3:
    print("二或三")
case 4...10:
    print("介於 4~10")
default:
    print("其他")
}

for i in 1...5 {
    print(i)   // 1,2,3,4,5
}

for i in 1..<5 {
    print(i)   // 1,2,3,4
}

var m = 3
while m > 0 {
    print(m)
    m -= 1
}

class Person {
    var name: String
    var age: Int
    
    init(name: String, age: Int) {
        self.name = name
        self.age = age
    }
    
    func sayHello() {
        print("Hi, I'm \(name), \(age) years old.")
    }
}
let p = Person(name: "Tom", age: 18)
p.sayHello()

var str: String? = nil
print(str?.count)       // 安全呼叫 -> nil
print(str ?? "default") // 預設值