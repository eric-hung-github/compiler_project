class Test {

    fun checkPrimeNumber(num: int): int {
        println(num)

        num = num / 2

        var i = 2
        var isprime: int

        while (i <= num ) {
            print(i)

            isprime = num % i

            print(": ")

            print(isprime)


            if (isprime == 0) {
                println(" false ")
                return 0
            }
            i = i + 1
        }

        println(" true ")

        return 1
    }

    fun main() {
        var low = 20
        val high = 50
        var result:int

        while (low < high) {
            result = checkPrimeNumber(low) 
            if (result == 1){
                println(low)
                println("===========")
            }

            low = low + 1
        }
    }


}