val x = 20 //Constant
var y = 20

val a: Int = 5
val b: Double = 3.14
val c: String = "Hello"
val d: Boolean = true
val e: Char = 'E'

var f: Int = b.toInt()

fun add(x: Int, y: Int): Int {
    return x + y
}

fun sub(x: Int, y: Int) = x - y

val n = 10
if (n > 5) {
    println("大於 5")
} else {
    println("小於等於 5")
}

// when 相當於 switch
when (n) {
    1 -> println("一")
    2, 3 -> println("二或三")
    in 4..10 -> println("介於 4~10")
    else -> println("其他")
}

for (i in 1..5) {
    println(i)   // 1,2,3,4,5
}

for (i in 1 until 5) {
    println(i)   // 1,2,3,4
}

while (n > 0) {
    println(n)
}

class Person(val name: String, var age: Int) {
    fun sayHello() {
        println("Hi, I'm $name, $age years old.")
    }
}
val p = Person("Tom", 18)
p.sayHello()

var str: String? = null
println(str?.length)     // 安全呼叫, 輸出 null
println(str ?: "default") // Elvis 運算子