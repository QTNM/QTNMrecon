// Example boiler-plate code for a pipeline module

// std includes
#include <string>

// tool includes

// must have include for pipeline
#include <Event.hh>

class ExampleModule
{
    public:
        ExampleModule(std::string inbox, std::string outbox); // constructor; required
        // Configures the module. Could have more config parameters
        // Minimum required are the key labels for input and output 
        // of DataPack data item. String Key inbox label and new Key outbox label.

        DataPack operator()(DataPack dp); // this is called by the pipeline
        // any module in the pipeline should be able to receive a
        // DataPack and return a DataPack. 
        // Only Readers should expect no input argument and return a DataPack
        // and Writers only receive a DataPack and return void as signature.

        // getter/setter methods for configuration could live here.

    private:
    // include any configuration data members for internal use here.

    // these below serve as string keys to access (read/write) the DataPack
    std::string inkey;
    std::string outkey;

};
