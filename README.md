# FireDLL

A small program which allows you to invoke exported dll functions directly from your command line. 

# Usage
```cmd
FireDLL.exe <DLL> <Export Function name> <return type> <args...>
```

# Return types
The return type dicatates how FireDLL should display the function's output.

#### Example Return Types
```
bool
byte
wchar
short
int
long
ushort
uint
ulong
bool*
byte*
char*
wchar*
string*
wstring*
short*
int*
long*
ushort*
uint*
ulong*
```

# Example usage
```cmd
FireDLL.exe user32.dll MessageBoxA int nullptr "body text" "This is a title" 20
```

The above example will invoke the narrow string variant of the message box function present within the Windows API. 
However, some windows api functions such as ```MessageBoxW``` use wide strings instead of narrow strings. This will cause issues with displaying text if narrow strings as provided as input.
But FireDLL has built-in functionality for converting narrow strings to wide strings.

```cmd
FireDLL.exe user32.dll MessageBoxW int nullptr wstr "body text" wstr "This is a title" 20
```
Using the ```wstr``` prefix before a string will convert the string to wide automatically before providing it to the function.

# Disclaimer
As described in the software license terms, **This software comes with absolutely NO WARRANTY and that any software faults and/or software malfunctions that may CAUSE DAMAGE are YOUR RESPONSIBILITY and the software author will NOT BE AT FAULT. YOU USE THIS SOFTWARE AT YOUR OWN RISK.**

**THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.**

**Exported DLL functions can be dangerous if used improperly, you use this software at your own risk.**
**Windows API Functions such as filesystem operations should not be used.**
