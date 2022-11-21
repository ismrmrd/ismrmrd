# Session Protocol
The MR Data (MRD) streaming protocol describes the communication of data ([k-space](mrd_raw_data.md), [image](mrd_image_data.md), or [waveform](mrd_waveform_data.md)) between a client and a server pair.  It consists of a series of [messages](mrd_messages.md) that are sent through a TCP/IP socket in sessions with the protocol defined as follow:
1. The server is started and listens for incoming connections on a designated TCP port (9002 by default).  A client initiates a session by connecting to the TCP port above.
1. The client sends a configuration message to indicate analysis that should be performed by the server.  The message may be either (but not both):
    - [MRD_MESSAGE_CONFIG_FILE](MRD_MESSAGE_CONFIG_FILE), corresponding to the name of a config file that exists on the server
    - [MRD_MESSAGE_CONFIG_TEXT](MRD_MESSAGE_CONFIG_TEXT), configuration parameters for the server, in XML text format.
1. The client sends the MRD acquisition parameter header, [MRD_MESSAGE_HEADER](MRD_MESSAGE_HEADER), containing information pertaining to the entire acquisition.  This information is sent as XML formatted text that conforms to a standardized [ismrmrd.xsd schema](../schema/ismrmrd.xsd).
1. The client sends raw k-space, image, waveform or text data.  Not all types of data may be sent, depending on the analysis pipeline to be performed.  For example, image processing pipelines may not contain k-space or waveform data.  Data of each type must sent in order by acquisition time, but the order between messages of different types is not guaranteed.  For eample, a waveform message corresponding to a specific time may be sent before or after the raw k-space data from that time.  The data types are:
    - [MRD_MESSAGE_ACQUISITION](MRD_MESSAGE_ACQUISITION): Data from a single k-space readout, including a fixed raw data [AcquisitionHeader](AcquisitionHeader) and optional k-space trajectory information.
    - [MRD_MESSAGE_IMAGE](MRD_MESSAGE_IMAGE): Image data as a 2D or 3D array, including both a fixed image data [ImageHeader](ImageHeader) and a flexible set of image [MetaAttributes](MetaAttributes) formatted as an XML text string.
    - [MRD_MESSAGE_WAVEFORM](MRD_MESSAGE_WAVEFORM): Waveform data such as physiological monitoring (ECG, pulse oximeter, respiratory motion) including a fixed waveform [WaveformHeader](WaveformHeader).
    - [MRD_MESSAGE_TEXT](MRD_MESSAGE_TEXT): Informational text for the other party.  This text may provide logging information about the status of the analysis or client, but is optional and should not be used for workflow control.
1. At any point after a config message is received, the server may send back raw k-space, image, waveform, or text data as described above.
1. When all data has been sent, the client sends [MRD_MESSAGE_CLOSE](MRD_MESSAGE_CLOSE) to indicate that no further data will be sent by the client.
1. When the server has sent all data from its side, it also sends [MRD_MESSAGE_CLOSE](MRD_MESSAGE_CLOSE).  This usually occurs after the client has sent MRD_MESSAGE_CLOSE, but can also occur if the server encounters an unrecoverable error and no further data can be processed.
1.  The TCP session may be closed by either side and the MRD streaming session is complete.

```{mermaid}

sequenceDiagram
    autonumber
    rect rgba(0, 255, 0, 0.05)
        Note over Client,Server: Startup
        Client->>+Server: Establish connection
        Client-->>Server: Configuration information (xml text or filename)
        Client-->>Server: MRD header (xml text)
    end

    rect rgba(0, 0, 255, 0.05)
        Note over Client,Server: Data transfer
        par
            loop Sending data
                Client-->>+Server: k-space, image, waveform, text message data
            end
        and
            loop Receiving data
                Server-->>-Client: k-space, image, waveform, text message data
            end
        end
    end

    rect rgba(255, 0, 0, 0.05)
        Note over Client,Server: Shutdown
        par
            Client->>Server: Session complete message
        and
            Server->>-Client: Session complete message
        end
        Server->Client: TCP session closed by either side
    end
```
