#pragma once

class ResourceState {
public:
    /// state codes
    enum Code {
        Initial,    ///< resource has just been created
        Setup,      ///< the resource has a setup object, but is not loaded
        Pending,    ///< resource is pending (asynchronous loading)
        Valid,      ///< resource has become valid
        Failed,     ///< resource creation has failed

        NumStates,
        InvalidState,
    };

    /// convert resource State to string
    static const char* toString(Code c);
    /// convert string to resource State
    static Code fromString(const char* str);
};
