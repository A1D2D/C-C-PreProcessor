#include <iostream>
#include "CPreProcessor.h"

//* This example cpp shows how to use/test the CPreProcessor class
//* The class is most usefull to implement in other languages or
//* problems where a macro or include system is needed

//* issue: there is a quite big issue with the Expression parser part
//* because it can only check for value much and no text

std::string LIB1Code = R"(
   //libFileCode
   #pragma once
   void function(int value) {
   }
   #ifdef PRINT
      void print(float f) {
         printf(f);
      }
   #endif
   )";

std::string mainCode = R"(
   //mainCode
   #include <LIB1>

   int main() {
   }
)";

int main() {

   CPreProcessor processor;
   processor.recursiveCallCountLim = 10; //sets how many recursive macro cals can happen
   processor.macroCallCountLim = 100;  //sets how many times macros can be called
   processor.macroDefCountLim = 100; //sets the maximum count of macros

   processor.sysDependencies.push_back({LIB1Code,"LIB1"}); //defines LIB1Code as a system library with the name LIB1,
   //use dependencies instead if sysDependencies, if you want to include simple files
   //in that case use "" to include the file else include system libs using <>

   //processor.setKeyPrefix("//#"); //The set prefix allows to use keyword indicators other than #

   processor.define("PRINT","");//defines a macro for the preProcessor with no values/args

   processor.preProcess(mainCode, "simple"); //preprocess the code if it had a name simple

   if (processor.errorLog.empty()) { //checking error log
      std::cout << "preProc: \n" << mainCode << std::endl; //print result codee

      std::unordered_map<std::string, CPreProcessor::MacroDef> macros = processor.macros; //print macros
      std::cout << "Macros: " << std::endl;
      for (const auto& pair : macros) {
         auto& key = pair.first;
         auto& value = pair.second;
         std::cout << "Name: " << key << ", Args: ";
         for (const auto& num : value.args) {
            std::cout << num << ",";
         }
         std::cout << " Value: " << value.macroAssembly << std::endl;
      }
   } else {
      for (const auto & eLID : processor.errorLog) { //print errors to console
         std::cout << "error: " << eLID << std::endl;
      }
   }
   return 0;
}
