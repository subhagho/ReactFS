/*!
 *  Auto-generated types for schema definition [source schema: test_schema]
 *
 *  Reference Schema Version : 0001.0000
 *  Generated by : subho
 *  Generated on : 2016-12-27 16:52:56.852
 */


#ifndef TEST_TYPE_H
#define TEST_TYPE_H
#include "types/includes/type_defs.h"
#include "types/includes/__base_type.h"



using namespace REACTFS_NS_COMMON_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX::types;



namespace com {
	namespace wookler {
		namespace test {
			class test_type  : public com::wookler::reactfs::core::types::__base_type
			{
			private:
				char * testString = nullptr;

				double * testDouble = nullptr;

				float * testFloat = nullptr;

				vector<char *> * testListString = nullptr;

				unordered_map<double, char *> * testMapString = nullptr;



				void set_value_testString ( __struct_datatype__ *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						__struct_datatype__::const_iterator iter = __data->find ( "testString" );
						if ( iter != __data->end() ) {
							void *__ptr = iter->second;
							if ( NOT_NULL ( __ptr ) ) {
								char * __var = static_cast<char *> ( __ptr );
								CHECK_CAST ( __var, TYPE_NAME ( void * ), TYPE_NAME ( char ) );
								this->testString = __var;

							} else {
								this->testString = nullptr;
							}
						} else {
							this->testString = nullptr;
						}
					} else {
						this->testString = nullptr;
					}
				}

				void set_map_testString ( __struct_datatype__ *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->testString ) ) {
						__data->insert ( {"testString", this->testString} );

					}
				}

				void set_value_testDouble ( __struct_datatype__ *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						__struct_datatype__::const_iterator iter = __data->find ( "testDouble" );
						if ( iter != __data->end() ) {
							void *__ptr = iter->second;
							if ( NOT_NULL ( __ptr ) ) {
								double * __var = static_cast<double *> ( __ptr );
								CHECK_CAST ( __var, TYPE_NAME ( void * ), TYPE_NAME ( double ) );
								this->testDouble = __var;

							} else {
								this->testDouble = nullptr;
							}
						} else {
							this->testDouble = nullptr;
						}
					} else {
						this->testDouble = nullptr;
					}
				}

				void set_map_testDouble ( __struct_datatype__ *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->testDouble ) ) {
						__data->insert ( {"testDouble", this->testDouble} );

					}
				}

				void set_value_testFloat ( __struct_datatype__ *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						__struct_datatype__::const_iterator iter = __data->find ( "testFloat" );
						if ( iter != __data->end() ) {
							void *__ptr = iter->second;
							if ( NOT_NULL ( __ptr ) ) {
								float * __var = static_cast<float *> ( __ptr );
								CHECK_CAST ( __var, TYPE_NAME ( void * ), TYPE_NAME ( float ) );
								this->testFloat = __var;

							} else {
								this->testFloat = nullptr;
							}
						} else {
							this->testFloat = nullptr;
						}
					} else {
						this->testFloat = nullptr;
					}
				}

				void set_map_testFloat ( __struct_datatype__ *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->testFloat ) ) {
						__data->insert ( {"testFloat", this->testFloat} );

					}
				}

				void set_value_testListString ( __struct_datatype__ *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						__struct_datatype__::const_iterator iter = __data->find ( "testListString" );
						if ( iter != __data->end() ) {
							void *__ptr = iter->second;
							if ( NOT_NULL ( __ptr ) ) {
								vector<char *> * __var = static_cast<vector<char *> *> ( __ptr );
								CHECK_CAST ( __var, TYPE_NAME ( void * ), TYPE_NAME ( vector ) );
								this->testListString = __var;

							} else {
								this->testListString = nullptr;
							}
						} else {
							this->testListString = nullptr;
						}
					} else {
						this->testListString = nullptr;
					}
				}

				void set_map_testListString ( __struct_datatype__ *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->testListString ) ) {
						__data->insert ( {"testListString", this->testListString} );

					}
				}

				void set_value_testMapString ( __struct_datatype__ *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						__struct_datatype__::const_iterator iter = __data->find ( "testMapString" );
						if ( iter != __data->end() ) {
							void *__ptr = iter->second;
							if ( NOT_NULL ( __ptr ) ) {
								unordered_map<double, char *> * __var = static_cast<unordered_map<double, char *> *> ( __ptr );
								CHECK_CAST ( __var, TYPE_NAME ( void * ), TYPE_NAME ( unordered_map ) );
								this->testMapString = __var;

							} else {
								this->testMapString = nullptr;
							}
						} else {
							this->testMapString = nullptr;
						}
					} else {
						this->testMapString = nullptr;
					}
				}

				void set_map_testMapString ( __struct_datatype__ *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->testMapString ) ) {
						__data->insert ( {"testMapString", this->testMapString} );

					}
				}


			public:
				void set_testString ( char * testString )
				{
					this->testString = testString;
				}

				char * get_testString ( void )
				{
					return this->testString;
				}

				void set_testString ( string &testString )
				{
					PRECONDITION ( this->__is_allocated == true );
					FREE_PTR ( this->testString );
					if ( !IS_EMPTY ( testString ) ) {
						uint32_t __size = testString.length() + 1;
						this->testString = ( char * ) malloc ( sizeof ( char ) * __size );
						CHECK_ALLOC ( this->testString, TYPE_NAME ( char * ) );
						memset ( this->testString, 0, __size );
						memcpy ( this->testString, testString.c_str(), ( __size - 1 ) );
					}
				}

				void set_testDouble ( double * testDouble )
				{
					this->testDouble = testDouble;
				}

				double * get_testDouble ( void )
				{
					return this->testDouble;
				}

				void set_testDouble ( double &testDouble )
				{
					PRECONDITION ( this->__is_allocated == true );
					FREE_PTR ( this->testDouble );
					this->testDouble = ( double * ) malloc ( sizeof ( double ) );
					CHECK_ALLOC ( this->testDouble, TYPE_NAME ( double ) );
					* ( this->testDouble ) = testDouble;
				}

				void set_testFloat ( float * testFloat )
				{
					this->testFloat = testFloat;
				}

				float * get_testFloat ( void )
				{
					return this->testFloat;
				}

				void set_testFloat ( float &testFloat )
				{
					PRECONDITION ( this->__is_allocated == true );
					FREE_PTR ( this->testFloat );
					this->testFloat = ( float * ) malloc ( sizeof ( float ) );
					CHECK_ALLOC ( this->testFloat, TYPE_NAME ( float ) );
					* ( this->testFloat ) = testFloat;
				}

				vector<char *> * get_testListString ( void )
				{
					return this->testListString;
				}

				void set_testListString ( vector<char *> * testListString )
				{
					this->testListString = testListString;
				}

				void add_to_testListString ( string &testListString )
				{
					PRECONDITION ( this->__is_allocated == true );
					if ( IS_NULL ( this->testListString ) ) {
						this->testListString = new std::vector<char *>();
						CHECK_ALLOC ( this->testListString, TYPE_NAME ( vector ) );
					}
					if ( !IS_EMPTY ( testListString ) ) {
						uint32_t __size = testListString.length() + 1;
						char *__var = ( char * ) malloc ( sizeof ( char ) * __size );
						CHECK_ALLOC ( __var, TYPE_NAME ( char * ) );
						memset ( __var, 0, __size );
						memcpy ( __var, testListString.c_str(), ( __size - 1 ) );
						this->testListString->push_back ( __var );
					}
				}

				unordered_map<double, char *> * get_testMapString ( void )
				{
					return this->testMapString;
				}

				void set_testMapString ( unordered_map<double, char *> * testMapString )
				{
					this->testMapString = testMapString;
				}

				void add_to_testMapString ( double m_key, string &m_value )
				{
					PRECONDITION ( this->__is_allocated == true );
					if ( IS_NULL ( this->testMapString ) ) {
						this->testMapString = new std::unordered_map<double, char *>();
						CHECK_ALLOC ( this->testMapString, TYPE_NAME ( unordered_map ) );
					}
					CHECK_NOT_EMPTY ( m_value );
					uint32_t __size = ( m_value.length() + 1 ) * sizeof ( char );
					char *__var = ( char * ) malloc ( sizeof ( char ) * __size );
					CHECK_ALLOC ( __var, TYPE_NAME ( char * ) );
					memset ( __var, 0, __size );
					memcpy ( __var, m_value.c_str(), ( __size - 1 ) );
					this->testMapString->insert ( {m_key, __var} );
				}

				test_type()
				{
					this->__is_allocated = true;
					this->testString = nullptr;
					this->testDouble = nullptr;
					this->testFloat = nullptr;
					this->testListString = nullptr;
					this->testMapString = nullptr;

				}

				test_type ( __struct_datatype__ *__data )
				{
					CHECK_NOT_NULL ( __data );
					this->__is_allocated = false;
					this->testString = nullptr;
					this->testDouble = nullptr;
					this->testFloat = nullptr;
					this->testListString = nullptr;
					this->testMapString = nullptr;

					this->deserialize ( __data );
				}

				~test_type()
				{
					if ( this->__is_allocated ) {
						FREE_PTR ( this->testString );
					}

					if ( this->__is_allocated ) {
						FREE_PTR ( this->testDouble );
					}

					if ( this->__is_allocated ) {
						FREE_PTR ( this->testFloat );
					}

					if ( this->__is_allocated ) {
						FREE_NATIVE_LIST ( this->testListString );
					} else {
						this->testListString->clear();
						CHECK_AND_FREE ( this->testListString );
					}

					if ( this->__is_allocated ) {
						FREE_NATIVE_MAP ( this->testMapString );
					} else {
						this->testMapString->clear();
						CHECK_AND_FREE ( this->testMapString );
					}


				}

				void deserialize ( __struct_datatype__ *__data ) override
				{
					CHECK_NOT_NULL ( __data );
					this->set_value_testString ( __data );

					this->set_value_testDouble ( __data );

					this->set_value_testFloat ( __data );

					this->set_value_testListString ( __data );

					this->set_value_testMapString ( __data );


				}

				__struct_datatype__ *serialize() override
				{
					__struct_datatype__ *__data = new __struct_datatype__();
					CHECK_ALLOC ( __data, TYPE_NAME ( __struct_datatype__ ) );
					this->set_map_testString ( __data );

					this->set_map_testDouble ( __data );

					this->set_map_testFloat ( __data );

					this->set_map_testListString ( __data );

					this->set_map_testMapString ( __data );


					return __data;
				}


			};

		}

	}

}

#endif // TEST_TYPE_H

