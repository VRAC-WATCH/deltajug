/* -*-c++-*-
 * testClient - main (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2007-2008, MOVES Institute
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *  Pjotr van Amerongen
 */

#include <dtCore/globals.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

#include <iostream>

#include "testClient.h"

//////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
   dtCore::RefPtr<EchoClient> app;

   dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_INFO);

   try
   {
      app = new EchoClient();
      app->Config();
      app->Run();
   }
   catch (dtUtil::Exception& ex)
   {
      std::cout << "Caught Exception: " << ex.ToString() << std::endl;
   }

   return 0;
}
