/*
 * Author: Matthew Craven, mcraven2015@my.fit.edu
 * Author: Michael Mascari, mmascari2017@my.fit.edu
 * Course: CSE 4250, Spring 2019
 * Project: Proj2, Simulation (TM)
 * Implementation:  Kotlin version 1.3.50-release-112 (JRE 11.0.4+11)
 */

import java.io.File
import java.io.BufferedReader
import java.io.InputStreamReader
import java.util.ArrayDeque

fun nextWord(str : String, fromIndex : Int) : Pair<Int, Int> {
  var index : Int = fromIndex
  while ( index < str.length && str.get(index) == ' ' ) {
    index += 1
  }
  val wordStartIndex : Int = index

  while ( index < str.length && str.get(index) != ' ' ) {
    index += 1
  }

  return Pair(wordStartIndex, index)
}

fun substringPair(str : String, bounds : Pair<Int, Int>) : String {
  return str.substring(bounds.first, bounds.second)
}

enum class Action {
  MoveRight, MoveLeft, Accept, Reject
}

class TurningMachine (descLines : List<String>) {
  val numStates : Int = descLines[0].split(" ")[0].toInt()
  val alphabet : String = descLines[0].split(" ")[1]
  val blank : Int = alphabet.length - 1


  private class StateInfo (a : Action, i : Array<Int>,
                           c : Array<Int>, s : String) {
    val action : Action = a
    val nextStates : Array<Int> = i
    val charsToWrite : Array<Int> = c
      // Store the numbers of the characters, for easier lookup
    val comment : String = s
  }

  private fun parseLine(line : String) : StateInfo {
    var currWordBounds : Pair<Int, Int> = Pair(0, 0)

    // Ignore the state number
    currWordBounds = nextWord(line, currWordBounds.second)

    //Action to take in current state
    currWordBounds = nextWord(line, currWordBounds.second)
    val action : Action = when (substringPair(line, currWordBounds)) {
      "L" -> Action.MoveLeft
      "R" -> Action.MoveRight
      "Y" -> Action.Accept
      "N" -> Action.Reject
      else -> throw Error("Bad TM action format")
    }

    val nextStates : Array<Int> = Array(alphabet.length) {
      currWordBounds = nextWord(line, currWordBounds.second);
      substringPair(line, currWordBounds).toInt()
    }

    val charsToWrite : Array<Int> = Array(alphabet.length) {
      currWordBounds = nextWord(line, currWordBounds.second);
      if (currWordBounds.second - currWordBounds.first != 1) {
        throw Error("Bad TM write-character format")
      }
      // Store the numbers of the characters, for easier lookup
      alphabet.indexOf(line.get(currWordBounds.first))
    }

    // Find the start of the comment
    currWordBounds = nextWord(line, currWordBounds.second)
    val comment : String = line.substring(currWordBounds.first)

    //println(comment)

    return StateInfo(action, nextStates, charsToWrite, comment)
  }

  private val rawActionTable : Array<StateInfo> = Array(numStates) {
    i -> parseLine(descLines[i+1])
  }

  fun getID(c : Char) : Int {
    return alphabet.indexOf(c)
  }

  fun getAction(stateNum : Int) : Action {
    return rawActionTable[stateNum].action
  }

  fun getNextState(stateNum : Int, charID : Int) : Int {
    return rawActionTable[stateNum].nextStates[charID]
  }

  fun getCharToWrite(stateNum: Int, charID : Int) : Int {
    return rawActionTable[stateNum].charsToWrite[charID]
  }

  fun getComment(stateNum : Int) : String {
    return rawActionTable[stateNum].comment
  }
}

fun main(args: Array<String>) {
  if (args.size == 0) {
    println("provide filename of TM description as arg")
    return
  }

  val ourTM : TurningMachine = TurningMachine(File(args[0]).readLines())

  // It took 30 minutes to figure out how to forEach over stdin. Yikes!
  BufferedReader(InputStreamReader(System.`in`)).forEachLine {
    var currState : Int = 0
    var stepNum : Int = 0
    var leftTape  : ArrayDeque<Int> = ArrayDeque()
    var rightTape : ArrayDeque<Int> = ArrayDeque()

    //Safely pop from correct end of tape, putting in a blank if needed
    fun scrollLeft() : Int {
      if (leftTape.isEmpty()) { leftTape.addLast(ourTM.blank) }
      return leftTape.pollLast()
    }
    fun scrollRight() : Int {
      if (rightTape.isEmpty()) { rightTape.addFirst(ourTM.blank) }
      return rightTape.pollFirst()
    }

    it.forEach {
      rightTape.addLast(ourTM.getID(it))
    }
    leftTape.addFirst(ourTM.blank)
    rightTape.addLast(ourTM.blank)
    var currSymbol : Int = scrollRight()
    var finished : Boolean = false

    while (true) {
      leftTape.addLast(currSymbol)
      while (!leftTape.isEmpty() && leftTape.getFirst() == ourTM.blank) {
        leftTape.removeFirst() // remove all leading blanks
      }
      leftTape.addFirst(ourTM.blank) // put in one leading blank
      leftTape.removeLast() // safe due to the addFirst()

      rightTape.addFirst(currSymbol)
      while (!rightTape.isEmpty() && rightTape.getLast() == ourTM.blank) {
        rightTape.removeLast() // remove all trailing blanks
      }
      rightTape.addLast(ourTM.blank) // put in one trailing blank
      rightTape.removeFirst() // safe due to the addLast()

      print(stepNum++) // because Michael is a C programmer
      print(": state=")
      print(currState)
      print(" ")

      for (j in leftTape) {
        print(" ")
        print(ourTM.alphabet.get(j))
      }
      print(" [")
      print(ourTM.alphabet.get(currSymbol))
      print("]")
      for (j in rightTape) { //backwards
        print(" ")
        print(ourTM.alphabet.get(j))
      }
      print("    ")
      print(ourTM.getComment(currState))
      print("\n")

      if (finished) {
        break
      }

      var nextState : Int = ourTM.getNextState(currState, currSymbol)
      currSymbol = ourTM.getCharToWrite(currState, currSymbol)
      currState = nextState

      finished = when (ourTM.getAction(currState)) {
        Action.Accept, Action.Reject -> true
        Action.MoveLeft -> {
          rightTape.addFirst(currSymbol)
          currSymbol = scrollLeft()
          false
        }
        Action.MoveRight -> {
          leftTape.addLast(currSymbol)
          currSymbol = scrollRight()
          false
        }
      }
    }

    println() //Extra line break between test cases
  }
}
