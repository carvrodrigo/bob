/**
 * @file src/cxx/io/io/TensorFile.h
 * @author <a href="mailto:Laurent.El-Shafey@idiap.ch">Laurent El Shafey</a> 
 *
 * @brief This class can be used to load and store arrays from/to .tensor files
 */

#ifndef TORCH_IO_TENSORFILE_H
#define TORCH_IO_TENSORFILE_H

#include "core/cast.h"
#include "io/TensorFileHeader.h"
#include "io/InlinedArrayImpl.h"
#include "io/Exception.h"

namespace Torch {
  /**
   * \ingroup libio_api
   * @{
   *
   */
  namespace io {

    /**
     * Defines the flags that might be used when loading/storing a file
     * containing blitz arrays.
     */
    enum _TensorFileFlag {
      _unset   = 0,
      _append  = 1L << 0,
      _in      = 1L << 3,
      _out     = 1L << 4
    };

    /**
     * This class can be used for loading and storing multiarrays from/to
     * tensor files
     */
    class TensorFile
    {
      public:
        /**
         * Defines the bitmask type for providing information about the type of
         * the stream.
         */
        typedef _TensorFileFlag openmode;
        static const openmode append  = _append;
        static const openmode in      = _in;
        static const openmode out     = _out; 

        /**
         * Constructor
         */
        TensorFile(const std::string& filename, openmode f);

        /**
         * Destructor
         */
        ~TensorFile();

        /**
         * Tests if next operation will succeed.
         */
        inline bool operator!() const { return !m_stream; }

        /**
         * Closes the TensorFile
         */
        void close();

        /** 
         * Puts an Array of a given type into the output stream/file. If the
         * type/shape have not yet been set, it is set according to the type
         * and shape given in the blitz array, otherwise the type/shape should
         * match or an exception is thrown.
         *
         * Please note that blitz::Array<> will be implicitly constructed as
         * required and respecting those norms.
         *
         * @warning: Please convert your files to HDF5, this format is
         * deprecated starting on 16.04.2011 - AA
         */
        void write(const detail::InlinedArrayImpl& data);

        /**
         * A shortcut to write a blitz::Array<T,D>
         *
         * @warning: Please convert your files to HDF5, this format is
         * deprecated starting on 16.04.2011 - AA
         */
        template <typename T, int D> 
          inline void write(blitz::Array<T,D>& bz) {
          write(detail::InlinedArrayImpl(bz));
        }

        /**
         * Load one blitz++ multiarray from the input stream/file All the
         * multiarrays saved have the same dimensions.
         */
        template <typename T, int D> inline blitz::Array<T,D> read() {
          return read().cast<T,D>(); 
        }
 
        template <typename T, int D> inline blitz::Array<T,D> read(size_t
            index) { return read(index).cast<T,D>(); }
        
        detail::InlinedArrayImpl read(); 
        detail::InlinedArrayImpl read (size_t index);

        /**
         * Gets the Element type
         *
         * @warning An exception is thrown if nothing was written so far
         */
        inline Torch::core::array::ElementType getElementType() const { 
          headerInitialized(); 
          return m_header.m_elem_type; 
        }

        /**
         * Gets the number of dimensions
         *
         * @warning An exception is thrown if nothing was written so far
         */
        inline size_t getNDimensions() const {  
          headerInitialized(); 
          return m_header.m_n_dimensions; 
        }

        /**
         * Gets the shape of each array
         *
         * @warning An exception is thrown if nothing was written so far
         */
        inline const size_t* getShape() const { 
          headerInitialized(); 
          return m_header.m_shape; 
        }

        /**
         * Gets the shape of each array in a blitz format
         *
         * @warning An exception is thrown if nothing was written so far
         */
        template<int D> inline void getShape (blitz::TinyVector<int,D>& res) 
          const {
          headerInitialized(); 
          m_header.getShape(res);
        }

        /**
         * Gets the number of samples/arrays written so far
         *
         * @warning An exception is thrown if nothing was written so far
         */
        inline size_t size() const { 
          headerInitialized(); 
          return m_n_arrays_written; 
        }

        /**
         * Gets the number of elements per array
         *
         * @warning An exception is thrown if nothing was written so far
         */
        inline size_t getNElements() const { 
          headerInitialized(); 
          return m_header.getNElements(); 
        }

        /**
         * Gets the size along a particular dimension
         *
         * @warning An exception is thrown if nothing was written so far
         */
        inline size_t getSize(size_t dim_index) const { 
          headerInitialized(); 
          return m_header.getSize(dim_index); 
        }

        /**
         * Initializes the tensor file with the given type and shape.
         */
        inline void initTensorFile(Torch::core::array::ElementType type,
            size_t ndim, const size_t* shape) {
          initHeader(type, ndim, shape);
        }

      private: //Some stuff I need privately

        /**
         * Checks if the end of the tensor file is reached
         */
        inline void endOfFile() {
          if(m_current_array >= m_header.m_n_samples ) throw IndexError(m_current_array);
        }

        /** 
         * Checks that the header has been initialized, and raise an
         * exception if not
         */
        inline void headerInitialized() const { 
          if (!m_header_init) throw Uninitialized();
        }

        /**
         * Initializes the header of the (output) stream with the given type
         * and shape
         */
        void initHeader(Torch::core::array::ElementType type, size_t ndim,
            const size_t* shape);

      private: //representation

        bool m_header_init;
        size_t m_current_array;
        size_t m_n_arrays_written;
        std::fstream m_stream;
        detail::TensorFileHeader m_header;
        openmode m_openmode;
    };

    inline _TensorFileFlag operator&(_TensorFileFlag a, _TensorFileFlag b) { 
      return _TensorFileFlag(static_cast<int>(a) & static_cast<int>(b)); 
    }

    inline _TensorFileFlag operator|(_TensorFileFlag a, _TensorFileFlag b) { 
      return _TensorFileFlag(static_cast<int>(a) | static_cast<int>(b)); 
    }

    inline _TensorFileFlag operator^(_TensorFileFlag a, _TensorFileFlag b) { 
      return _TensorFileFlag(static_cast<int>(a) ^ static_cast<int>(b)); 
    }

    inline _TensorFileFlag& operator|=(_TensorFileFlag& a, _TensorFileFlag b) { 
      return a = a | b; 
    }

    inline _TensorFileFlag& operator&=(_TensorFileFlag& a, _TensorFileFlag b) { 
      return a = a & b; 
    }

    inline _TensorFileFlag& operator^=(_TensorFileFlag& a, _TensorFileFlag b) { 
      return a = a ^ b; 
    }

    inline _TensorFileFlag operator~(_TensorFileFlag a) { 
      return _TensorFileFlag(~static_cast<int>(a)); 
    }

  }
  /**
   * @}
   */
}

#endif /* TORCH_IO_BINFILE_H */
