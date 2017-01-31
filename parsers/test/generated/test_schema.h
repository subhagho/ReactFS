/*!
 *  Auto-generated types for schema definition [source schema: test_schema]
 *
 *  Reference Schema Version : 0001.0000
 *  Generated by : subho
 *  Generated on : 2017-01-31 15:33:05.950
 */


#ifndef TEST_SCHEMA_H
#define TEST_SCHEMA_H
#include "types/includes/type_defs.h"
#include "types/includes/__base_type.h"

#include "test_type.h"
#include "test_ref_type.h"


using namespace REACTFS_NS_COMMON_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX::types;



namespace com {
	namespace wookler {
		namespace test {
			/**
			* Generated code for data record type test_schema.
			*
			* Note:: Should not be modified as the changes will be lost when the code is re-generated.
			*/
			class test_schema  : public com::wookler::reactfs::core::types::__base_type
			{
			private:
				const char * key = nullptr;

				const long * testLong = nullptr;

				const short * shortWithDefault = nullptr;

				test_ref_type * testTypeRef = nullptr;

				std::vector<test_type *> * testListRef = nullptr;



				/**
				* Value key setter from the the serialized data map.
				*
				* @param __data - Serialized data map pointer.
				*/
				void read_value_key ( const record_struct *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						const void *__ptr = __data->get_field ( 0 );
						if ( NOT_NULL ( __ptr ) ) {
							// Set the value of key
							const char * __var = static_cast<const char *> ( __ptr );
							CHECK_CAST ( __var, TYPE_NAME ( void * ), TYPE_NAME ( char ) );
							this->key = __var;

						} else {
							this->key = nullptr;
						}
					} else {
						this->key = nullptr;
					}
				}

				/**
				 * Value testLong setter from the the serialized data map.
				 *
				 * @param __data - Serialized data map pointer.
				 */
				void read_value_testLong ( const record_struct *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						const void *__ptr = __data->get_field ( 1 );
						if ( NOT_NULL ( __ptr ) ) {
							// Set the value of testLong
							const long * __var = static_cast<const long *> ( __ptr );
							CHECK_CAST ( __var, TYPE_NAME ( void * ), TYPE_NAME ( long ) );
							this->testLong = __var;

						} else {
							this->testLong = nullptr;
						}
					} else {
						this->testLong = nullptr;
					}
				}

				/**
				 * Value shortWithDefault setter from the the serialized data map.
				 *
				 * @param __data - Serialized data map pointer.
				 */
				void read_value_shortWithDefault ( const record_struct *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						const void *__ptr = __data->get_field ( 2 );
						if ( NOT_NULL ( __ptr ) ) {
							// Set the value of shortWithDefault
							const short * __var = static_cast<const short *> ( __ptr );
							CHECK_CAST ( __var, TYPE_NAME ( void * ), TYPE_NAME ( short ) );
							this->shortWithDefault = __var;

						} else {
							this->shortWithDefault = nullptr;
						}
					} else {
						this->shortWithDefault = nullptr;
					}
				}

				/**
				 * Deserialize an instance of type test_ref_type from the
				 * passed input data.
				 *
				 * @param __input - Void pointer to input data of type record_struct
				 * @return - Deserialized instance pointer of type test_ref_type *
				 */
				test_ref_type * deserialize_test_ref_type ( const void *__input )
				{
					CHECK_NOT_NULL ( __input );
					const record_struct *__value = static_cast<const record_struct *> ( __input );
					CHECK_CAST ( __value, TYPE_NAME ( void * ), TYPE_NAME ( record_struct ) );
					test_ref_type * __data = new test_ref_type();
					__data->deserialize ( __value );
					CHECK_ALLOC ( __data, TYPE_NAME ( test_ref_type ) );
					return __data;
				}

				/**
				 * Value testTypeRef setter from the the serialized data map.
				 *
				 * @param __data - Serialized data map pointer.
				 */
				void read_value_testTypeRef ( const record_struct *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						const void *__ptr = __data->get_field ( 3 );
						if ( NOT_NULL ( __ptr ) ) {
							// Deserialize and set the value of testTypeRef
							test_ref_type * __var = deserialize_test_ref_type ( __ptr );
							CHECK_NOT_NULL ( __var );
							this->testTypeRef = __var;

						} else {
							this->testTypeRef = nullptr;
						}
					} else {
						this->testTypeRef = nullptr;
					}
				}

				/**
				 * Deserialize an instance of type test_type from the
				 * passed input data.
				 *
				 * @param __input - Void pointer to input data of type record_struct
				 * @return - Deserialized instance pointer of type test_type *
				 */
				test_type * deserialize_test_type ( const void *__input )
				{
					CHECK_NOT_NULL ( __input );
					const record_struct *__value = static_cast<const record_struct *> ( __input );
					CHECK_CAST ( __value, TYPE_NAME ( void * ), TYPE_NAME ( record_struct ) );
					test_type * __data = new test_type();
					__data->deserialize ( __value );
					CHECK_ALLOC ( __data, TYPE_NAME ( test_type ) );
					return __data;
				}

				/**
				 * Deserialize a list instance of type test_type * from the passed input data.
				 *
				 * @param __input - Void pointer to input data of type vector<record_struct *>
				 * @return - Deserialized pointer to list of type vector<test_type * *>
				 */
				std::vector<test_type *> *deserialize_list_std__vector_test_type___ ( const void *__input )
				{
					CHECK_NOT_NULL ( __input );
					const std::vector<record_struct *> *__value = static_cast<const std::vector<record_struct *> *> ( __input );
					CHECK_CAST ( __value, TYPE_NAME ( void * ), TYPE_NAME ( vector ) );

					std::vector<test_type *> *__list = new std::vector<test_type *>();
					CHECK_ALLOC ( __list, TYPE_NAME ( vector ) );
					for ( const record_struct *v : *__value ) {
						CHECK_NOT_NULL ( v );
						test_type * __tv = deserialize_test_type ( v );
						CHECK_NOT_NULL ( __tv );
						__list->push_back ( __tv );
					}
					return __list;
				}

				/**
				 * Value testListRef setter from the the serialized data map.
				 *
				 * @param __data - Serialized data map pointer.
				 */
				void read_value_testListRef ( const record_struct *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						const void *__ptr = __data->get_field ( 4 );
						if ( NOT_NULL ( __ptr ) ) {
							// Set the value of list testListRef
							std::vector<test_type *> *__var = deserialize_list_std__vector_test_type___ ( __ptr );
							CHECK_NOT_NULL ( __var );
							this->testListRef = __var;

						} else {
							this->testListRef = nullptr;
						}
					} else {
						this->testListRef = nullptr;
					}
				}


			public:
				/**
				* Get the pointer to the property key of type char *.
				* Returns a const pointer.
				*
				* @return char *
				*/
				const char * get_key ( void ) const
				{
					return this->key;
				}

				/**
				 * Get the pointer to the property testLong of type long *.
				 * Returns a const pointer.
				 *
				 * @return long *
				 */
				const long * get_testLong ( void ) const
				{
					return this->testLong;
				}

				/**
				 * Get the pointer to the property shortWithDefault of type short *.
				 * Returns a const pointer.
				 *
				 * @return short *
				 */
				const short * get_shortWithDefault ( void ) const
				{
					return this->shortWithDefault;
				}

				/**
				 * Get the pointer to the property testTypeRef of type test_ref_type *.
				 * Returns a const pointer.
				 *
				 * @return test_ref_type *
				 */
				const test_ref_type * get_testTypeRef ( void ) const
				{
					return this->testTypeRef;
				}

				/**
				 * Get the pointer to the property testListRef of type std::vector<test_type *> *.
				 * Returns a const pointer.
				 *
				 * @return std::vector<test_type *> *
				 */
				const std::vector<test_type *> * get_testListRef ( void ) const
				{
					return this->testListRef;
				}

				/**
				 * Create a new read-only instance of test_schema.
				 *
				 * Type to be used when instance is being de-serialized from record data.
				 */
				test_schema()
				{
					this->key = nullptr;
					this->testLong = nullptr;
					this->shortWithDefault = nullptr;
					this->testTypeRef = nullptr;
					this->testListRef = nullptr;

				}

				/**
				 * Descructor for test_schema
				 */
				~test_schema()
				{
					CHECK_AND_FREE ( this->testTypeRef );

					FREE_TYPE_LIST ( this->testListRef );


				}

				/**
				 * Deserialize this type instance from the passed data map.
				 *
				 * @param __data - Serialized data record instance to load this type from.
				 */
				void deserialize ( const record_struct *__data ) override
				{
					CHECK_NOT_NULL ( __data );

					this->record_type = __data->get_record_type();
					CHECK_NOT_NULL ( this->record_type );

					// Set the value of key from the serialized data map.
					this->read_value_key ( __data );

					// Set the value of testLong from the serialized data map.
					this->read_value_testLong ( __data );

					// Set the value of shortWithDefault from the serialized data map.
					this->read_value_shortWithDefault ( __data );

					// Set the value of testTypeRef from the serialized data map.
					this->read_value_testTypeRef ( __data );

					// Set the value of testListRef from the serialized data map.
					this->read_value_testListRef ( __data );



					this->__data = __data;
				}

				/**
				 * Compare this type instance to the passed instance.
				 *
				 * @param __target - Target instance to compare with. If pointer is not
				 * 			of the same type will throw exception.
				 * @return - Is equal?
				 */
				bool equals ( const void* __target ) const override
				{
					if ( IS_NULL ( __target ) ) {
						return false;
					}
					const test_schema * __t = static_cast<const test_schema *> ( __target );
					CHECK_CAST ( __t, TYPE_NAME ( void * ), TYPE_NAME ( test_schema * ) );

					{
						// Compare field key
						const __native_type *nt = get_field_type ( "key" );
						CHECK_NOT_NULL ( nt );
						__base_datatype_io *th = nt->get_type_handler ( __record_mode::RM_READ );
						CHECK_NOT_NULL ( th );
						const void *tv = __t->get_key();
						const void *sv = this->get_key();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
						}
						if ( NOT_NULL ( sv ) && NOT_NULL ( tv ) ) {
							bool r = th->compare ( tv, sv, __constraint_operator::EQ );
							if ( !r ) {
								return false;
							}
						}
					}

					{
						// Compare field testLong
						const __native_type *nt = get_field_type ( "testLong" );
						CHECK_NOT_NULL ( nt );
						__base_datatype_io *th = nt->get_type_handler ( __record_mode::RM_READ );
						CHECK_NOT_NULL ( th );
						const void *tv = __t->get_testLong();
						const void *sv = this->get_testLong();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
						}
						if ( NOT_NULL ( sv ) && NOT_NULL ( tv ) ) {
							bool r = th->compare ( tv, sv, __constraint_operator::EQ );
							if ( !r ) {
								return false;
							}
						}
					}

					{
						// Compare field shortWithDefault
						const __native_type *nt = get_field_type ( "shortWithDefault" );
						CHECK_NOT_NULL ( nt );
						__base_datatype_io *th = nt->get_type_handler ( __record_mode::RM_READ );
						CHECK_NOT_NULL ( th );
						const void *tv = __t->get_shortWithDefault();
						const void *sv = this->get_shortWithDefault();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
						}
						if ( NOT_NULL ( sv ) && NOT_NULL ( tv ) ) {
							bool r = th->compare ( tv, sv, __constraint_operator::EQ );
							if ( !r ) {
								return false;
							}
						}
					}

					{
						// Compare type instance testTypeRef
						const void* tv = __t->get_testTypeRef();
						const test_ref_type * sv = this->get_testTypeRef();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
							return true;
						}
						bool r = sv->equals ( tv );
						if ( !r ) {
							return false;
						}
					}

					{
						// Compare list instance testListRef
						const vector<test_type *> *tv = __t->get_testListRef();
						const vector<test_type *> *sv = this->get_testListRef();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
						}
						if ( NOT_NULL ( tv ) && NOT_NULL ( sv ) ) {
							if ( tv->size() != sv->size() ) {
								return false;
							}

							for ( uint32_t ii = 0; ii < tv->size(); ii++ ) {
								const test_type * v1 = ( *sv ) [ii];
								const test_type * v2 = ( *tv ) [ii];
								if ( IS_NULL ( v1 ) ) {
									if ( NOT_NULL ( v2 ) ) {
										return false;
									}
									continue;
								}
								// Check inner type equals.
								bool r = v1->equals ( v2 );
								if ( !r ) {
									return false;
								}
							}
						}
					}



					return true;
				}


			};


			/**
			 * Generated code for data record type mutable_test_schema.
			 *
			 * Note:: Should not be modified as the changes will be lost when the code is re-generated.
			 */
			class mutable_test_schema  : public com::wookler::reactfs::core::types::__mutable_base_type
			{
			private:
				char * key = nullptr;

				long * testLong = nullptr;

				short * shortWithDefault = nullptr;

				mutable_test_ref_type * testTypeRef = nullptr;

				std::vector<mutable_test_type *> * testListRef = nullptr;



				/**
				* Method extracts and set the value of key from
				* the serialized data map.
				*
				* @param __data - Serialized data map pointer.
				*/
				void set_serde_key ( mutable_record_struct *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->key ) ) {
						// Add the value of key to the data map.
						{
							const __native_type *ft = get_field_type ( "key" );
							CHECK_NOT_NULL ( ft );
							__data->add_field ( ft->get_index(), this->key );

						}

					}
				}

				/**
				 * Method extracts and set the value of testLong from
				 * the serialized data map.
				 *
				 * @param __data - Serialized data map pointer.
				 */
				void set_serde_testLong ( mutable_record_struct *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->testLong ) ) {
						// Add the value of testLong to the data map.
						{
							const __native_type *ft = get_field_type ( "testLong" );
							CHECK_NOT_NULL ( ft );
							__data->add_field ( ft->get_index(), this->testLong );

						}

					}
				}

				/**
				 * Method extracts and set the value of shortWithDefault from
				 * the serialized data map.
				 *
				 * @param __data - Serialized data map pointer.
				 */
				void set_serde_shortWithDefault ( mutable_record_struct *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->shortWithDefault ) ) {
						// Add the value of shortWithDefault to the data map.
						{
							const __native_type *ft = get_field_type ( "shortWithDefault" );
							CHECK_NOT_NULL ( ft );
							__data->add_field ( ft->get_index(), this->shortWithDefault );

						}

					}
				}

				/**
				 * Serialize the type value for mutable_test_ref_type and add to the data map.
				 *
				 * @param __value - Pointer to the field of type mutable_test_ref_type.
				 * @return - Serialized data map instance.
				 */
				mutable_record_struct *serialize_mutable_test_ref_type ( mutable_test_ref_type * __value )
				{
					CHECK_NOT_NULL ( __value );
					mutable_record_struct *__data = __value->serialize();
					CHECK_NOT_NULL ( __data );
					return __data;
				}

				/**
				 * Method extracts and set the value of testTypeRef from
				 * the serialized data map.
				 *
				 * @param __data - Serialized data map pointer.
				 */
				void set_serde_testTypeRef ( mutable_record_struct *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->testTypeRef ) ) {
						// Serialize the value of testTypeRef and add to the data map.
						{
							void *__ptr = serialize_mutable_test_ref_type ( this->testTypeRef );
							CHECK_NOT_NULL ( __ptr );

							const __native_type *ft = get_field_type ( "testTypeRef" );
							CHECK_NOT_NULL ( ft );

							__data->add_field ( ft->get_index(), __ptr );
						}

					}
				}

				/**
				 * Serialize the type value for mutable_test_type and add to the data map.
				 *
				 * @param __value - Pointer to the field of type mutable_test_type.
				 * @return - Serialized data map instance.
				 */
				mutable_record_struct *serialize_mutable_test_type ( mutable_test_type * __value )
				{
					CHECK_NOT_NULL ( __value );
					mutable_record_struct *__data = __value->serialize();
					CHECK_NOT_NULL ( __data );
					return __data;
				}

				/**
				 * Serialize the list instance of value type mutable_test_type *.
				 *
				 * @param __value - Vector instance of mutable_test_type *.
				 * @return - Vector of serialized data maps.
				 */
				std::vector<mutable_record_struct *> *serialize_list_std__vector_mutable_test_type___ ( std::vector<mutable_test_type *> *__value )
				{
					CHECK_NOT_EMPTY_P ( __value );
					std::vector<mutable_record_struct *> *__list = new std::vector<mutable_record_struct *>();
					CHECK_ALLOC ( __list, TYPE_NAME ( vector ) );
					for ( mutable_test_type * rec : * ( __value ) ) {
						CHECK_NOT_NULL ( rec );
						mutable_record_struct *rv = this->serialize_mutable_test_type ( rec );
						CHECK_NOT_NULL ( rv );
						__list->push_back ( rv );
					}
					return __list;
				}

				/**
				 * Method extracts and set the value of testListRef from
				 * the serialized data map.
				 *
				 * @param __data - Serialized data map pointer.
				 */
				void set_serde_testListRef ( mutable_record_struct *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->testListRef ) ) {
						// Serialize the list testListRef and add to the data map.
						{
							void *__ptr = serialize_list_std__vector_mutable_test_type___ ( this->testListRef );
							CHECK_NOT_NULL ( __ptr );

							const __native_type *ft = get_field_type ( "testListRef" );
							CHECK_NOT_NULL ( ft );

							__data->add_field ( ft->get_index(), __ptr );
						}

					}
				}


			public:
				/**
				* Set the pointer to key.
				*
				* @param key - Pointer of type char *.
				*/
				void set_key ( char * key )
				{
					this->key = key;
				}

				/**
				 * Get the pointer to the property key of type char *.
				 * Returns a const pointer.
				 *
				 * @return char *
				 */
				const char * get_key ( void ) const
				{
					return this->key;
				}

				/**
				 * Set the char buffer value from the specified string.
				 *
				 * Method should only be used when this instance is being
				 * used to create or update a new record instance.
				 *
				 * @param key - String value to set.
				 */
				void set_key ( const string &key )
				{
					FREE_PTR ( this->key );
					if ( !IS_EMPTY ( key ) ) {
						uint32_t __size = key.length() + 1;
						this->key = ( char * ) malloc ( sizeof ( char ) * __size );
						CHECK_ALLOC ( this->key, TYPE_NAME ( char * ) );
						memset ( this->key, 0, __size );
						memcpy ( this->key, key.c_str(), ( __size - 1 ) );
					}
				}

				/**
				 * Set the pointer to testLong.
				 *
				 * @param testLong - Pointer of type long *.
				 */
				void set_testLong ( long * testLong )
				{
					this->testLong = testLong;
				}

				/**
				 * Get the pointer to the property testLong of type long *.
				 * Returns a const pointer.
				 *
				 * @return long *
				 */
				const long * get_testLong ( void ) const
				{
					return this->testLong;
				}

				/**
				 * Set the value to testLong.
				 *
				 * Method should only be used when this instance is being
				 * used to create or update a new record instance.
				 *
				 * @Param testLong - Value reference.
				 */
				void set_testLong ( const long &testLong )
				{
					if ( IS_NULL ( this->testLong ) ) {
						this->testLong = ( long * ) malloc ( sizeof ( long ) );
						CHECK_ALLOC ( this->testLong, TYPE_NAME ( long ) );
					}
					* ( this->testLong ) = testLong;
				}

				/**
				 * Set the pointer to shortWithDefault.
				 *
				 * @param shortWithDefault - Pointer of type short *.
				 */
				void set_shortWithDefault ( short * shortWithDefault )
				{
					this->shortWithDefault = shortWithDefault;
				}

				/**
				 * Get the pointer to the property shortWithDefault of type short *.
				 * Returns a const pointer.
				 *
				 * @return short *
				 */
				const short * get_shortWithDefault ( void ) const
				{
					return this->shortWithDefault;
				}

				/**
				 * Set the value to shortWithDefault.
				 *
				 * Method should only be used when this instance is being
				 * used to create or update a new record instance.
				 *
				 * @Param shortWithDefault - Value reference.
				 */
				void set_shortWithDefault ( const short &shortWithDefault )
				{
					if ( IS_NULL ( this->shortWithDefault ) ) {
						this->shortWithDefault = ( short * ) malloc ( sizeof ( short ) );
						CHECK_ALLOC ( this->shortWithDefault, TYPE_NAME ( short ) );
					}
					* ( this->shortWithDefault ) = shortWithDefault;
				}

				/**
				 * Get the pointer to the property testTypeRef of type mutable_test_ref_type *.
				 * Returns a const pointer.
				 *
				 * @return mutable_test_ref_type *
				 */
				const mutable_test_ref_type * get_testTypeRef ( void ) const
				{
					return this->testTypeRef;
				}

				/**
				 * Set the pointer to testTypeRef.
				 *
				 * @param testTypeRef - Pointer of type mutable_test_ref_type *.
				 */
				void set_testTypeRef ( mutable_test_ref_type * testTypeRef )
				{
					this->testTypeRef = testTypeRef;
				}

				/**
				 * Get the pointer to the property testListRef of type std::vector<mutable_test_type *> *.
				 * Returns a const pointer.
				 *
				 * @return std::vector<mutable_test_type *> *
				 */
				const std::vector<mutable_test_type *> * get_testListRef ( void ) const
				{
					return this->testListRef;
				}

				/**
				 * Set the pointer to testListRef.
				 *
				 * @param testListRef - Pointer of type std::vector<mutable_test_type *> *.
				 */
				void set_testListRef ( std::vector<mutable_test_type *> * testListRef )
				{
					this->testListRef = testListRef;
				}

				/**
				 * Add a type element to the list testListRef.
				 * Elements are expected to be pre-allocated type
				 * pointer.
				 *
				 * Method should only be used when this instance is being
				 * used to create or update a new record instance.
				 *
				 * @param testListRef - String value to add to list.
				 */
				void add_to_testListRef ( mutable_test_type * testListRef )
				{
					CHECK_NOT_NULL ( testListRef );
					if ( IS_NULL ( this->testListRef ) ) {
						this->testListRef = new std::vector<mutable_test_type *>();
						CHECK_ALLOC ( this->testListRef, TYPE_NAME ( vector ) );
					}
					this->testListRef->push_back ( testListRef );
				}

				/**
				 * Empty constructor when creating an instance of mutable_test_schema for setting data locally.
				 * Should be used to create a new record instance of type mutable_test_schema.
				 *
				 * @param record_type - Parsed schema definition of this type.
				 */
				mutable_test_schema ( const __complex_type *record_type )
				{
					CHECK_NOT_NULL ( record_type );
					this->record_type = record_type;

					this->key = nullptr;
					this->testLong = nullptr;
					this->shortWithDefault = nullptr;
					this->testTypeRef = nullptr;
					this->testListRef = nullptr;

				}

				/**
				 * Copy constructor to create a copy instance of mutable_test_schema.
				 * Copy instances should be used to update existing records.
				 *
				 * @param source - Source instance of mutable_test_schema to copy from.
				 */
				mutable_test_schema ( const test_schema &source )
				{
					this->record_type = source.get_record_type();
					CHECK_NOT_NULL ( this->record_type );

					this->key = nullptr;
					this->testLong = nullptr;
					this->shortWithDefault = nullptr;
					this->testTypeRef = nullptr;
					this->testListRef = nullptr;

					// Set key from the source value.
					if ( NOT_NULL ( source.get_key() ) ) {
						string __key = string ( source.get_key() );
						this->set_key ( __key );
					}

// Set testLong from the source value.
					if ( NOT_NULL ( source.get_testLong() ) ) {
						this->set_testLong ( * ( source.get_testLong() ) );
					}

// Set shortWithDefault from the source value.
					if ( NOT_NULL ( source.get_shortWithDefault() ) ) {
						this->set_shortWithDefault ( * ( source.get_shortWithDefault() ) );
					}

// Set testTypeRef from the source value.
					if ( NOT_NULL ( source.get_testTypeRef() ) ) {
						this->testTypeRef = new mutable_test_ref_type ( source.get_testTypeRef() );
						CHECK_ALLOC ( this->testTypeRef, TYPE_NAME ( mutable_test_ref_type ) );
					}

// Deserialize the list testListRef from the source value.
					if ( NOT_EMPTY_P ( source.get_testListRef() ) ) {
						std::vector<mutable_test_type *> *__list = new std::vector<mutable_test_type *>();
						CHECK_ALLOC ( __list, TYPE_NAME ( vector ) );
						for ( const test_type * v : * ( source.get_testListRef() ) ) {
							CHECK_NOT_NULL ( v );
							mutable_test_type * __tv = new mutable_test_type ( v );
							CHECK_NOT_NULL ( __tv );
							__list->push_back ( __tv );
						}
						this->testListRef = __list;
					}


				}

				/**
				 * Copy constructor to create a copy instance of mutable_test_schema.
				 * Copy instances should be used to update existing records.
				 *
				 * @param source - Source instance of mutable_test_schema to copy from.
				 */
				mutable_test_schema ( const test_schema *source )
				{
					this->record_type = source->get_record_type();
					CHECK_NOT_NULL ( this->record_type );

					this->key = nullptr;
					this->testLong = nullptr;
					this->shortWithDefault = nullptr;
					this->testTypeRef = nullptr;
					this->testListRef = nullptr;

					// Set key from the source value.
					if ( NOT_NULL ( source->get_key() ) ) {
						string __key = string ( source->get_key() );
						this->set_key ( __key );
					}

// Set testLong from the source value.
					if ( NOT_NULL ( source->get_testLong() ) ) {
						this->set_testLong ( * ( source->get_testLong() ) );
					}

// Set shortWithDefault from the source value.
					if ( NOT_NULL ( source->get_shortWithDefault() ) ) {
						this->set_shortWithDefault ( * ( source->get_shortWithDefault() ) );
					}

// Set testTypeRef from the source value.
					if ( NOT_NULL ( source->get_testTypeRef() ) ) {
						this->testTypeRef = new mutable_test_ref_type ( source->get_testTypeRef() );
						CHECK_ALLOC ( this->testTypeRef, TYPE_NAME ( mutable_test_ref_type ) );
					}

// Deserialize the list testListRef from the source value.
					if ( NOT_EMPTY_P ( source->get_testListRef() ) ) {
						std::vector<mutable_test_type *> *__list = new std::vector<mutable_test_type *>();
						CHECK_ALLOC ( __list, TYPE_NAME ( vector ) );
						for ( const test_type * v : * ( source->get_testListRef() ) ) {
							CHECK_NOT_NULL ( v );
							mutable_test_type * __tv = new mutable_test_type ( v );
							CHECK_NOT_NULL ( __tv );
							__list->push_back ( __tv );
						}
						this->testListRef = __list;
					}


				}

				/**
				 * Descructor for mutable_test_schema
				 */
				~mutable_test_schema()
				{
					FREE_PTR ( this->key );

					FREE_PTR ( this->testLong );

					FREE_PTR ( this->shortWithDefault );

					CHECK_AND_FREE ( this->testTypeRef );

					FREE_TYPE_LIST ( this->testListRef );


				}

				/**
				 * Serialize this type instance into a data map instance.
				 *
				 * @return - Serialized data record instance.
				 */
				mutable_record_struct *serialize() override
				{
					mutable_record_struct *__data = new mutable_record_struct ( this->record_type );
					CHECK_ALLOC ( __data, TYPE_NAME ( mutable_record_struct ) );
					// Call the method to add the value of key to the serialized data map.
					this->set_serde_key ( __data );

// Call the method to add the value of testLong to the serialized data map.
					this->set_serde_testLong ( __data );

// Call the method to add the value of shortWithDefault to the serialized data map.
					this->set_serde_shortWithDefault ( __data );

// Call the method to add the value of testTypeRef to the serialized data map.
					this->set_serde_testTypeRef ( __data );

// Call the method to add the value of testListRef to the serialized data map.
					this->set_serde_testListRef ( __data );


					return __data;
				}

				/**
				 * Compare this type instance to the passed instance.
				 *
				 * @param __target - Target instance to compare with. If pointer is not
				 * 			of the same type will throw exception.
				 * @return - Is equal?
				 */
				bool equals ( const void* __target ) const override
				{
					if ( IS_NULL ( __target ) ) {
						return false;
					}
					const mutable_test_schema * __t = static_cast<const mutable_test_schema *> ( __target );
					CHECK_CAST ( __t, TYPE_NAME ( void * ), TYPE_NAME ( mutable_test_schema * ) );

					{
						// Compare field key
						const __native_type *nt = get_field_type ( "key" );
						CHECK_NOT_NULL ( nt );
						__base_datatype_io *th = nt->get_type_handler ( __record_mode::RM_READ );
						CHECK_NOT_NULL ( th );
						const void *tv = __t->get_key();
						const void *sv = this->get_key();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
						}
						if ( NOT_NULL ( sv ) && NOT_NULL ( tv ) ) {
							bool r = th->compare ( tv, sv, __constraint_operator::EQ );
							if ( !r ) {
								return false;
							}
						}
					}

					{
						// Compare field testLong
						const __native_type *nt = get_field_type ( "testLong" );
						CHECK_NOT_NULL ( nt );
						__base_datatype_io *th = nt->get_type_handler ( __record_mode::RM_READ );
						CHECK_NOT_NULL ( th );
						const void *tv = __t->get_testLong();
						const void *sv = this->get_testLong();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
						}
						if ( NOT_NULL ( sv ) && NOT_NULL ( tv ) ) {
							bool r = th->compare ( tv, sv, __constraint_operator::EQ );
							if ( !r ) {
								return false;
							}
						}
					}

					{
						// Compare field shortWithDefault
						const __native_type *nt = get_field_type ( "shortWithDefault" );
						CHECK_NOT_NULL ( nt );
						__base_datatype_io *th = nt->get_type_handler ( __record_mode::RM_READ );
						CHECK_NOT_NULL ( th );
						const void *tv = __t->get_shortWithDefault();
						const void *sv = this->get_shortWithDefault();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
						}
						if ( NOT_NULL ( sv ) && NOT_NULL ( tv ) ) {
							bool r = th->compare ( tv, sv, __constraint_operator::EQ );
							if ( !r ) {
								return false;
							}
						}
					}

					{
						// Compare type instance testTypeRef
						const void* tv = __t->get_testTypeRef();
						const mutable_test_ref_type * sv = this->get_testTypeRef();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
							return true;
						}
						bool r = sv->equals ( tv );
						if ( !r ) {
							return false;
						}
					}

					{
						// Compare list instance testListRef
						const vector<mutable_test_type *> *tv = __t->get_testListRef();
						const vector<mutable_test_type *> *sv = this->get_testListRef();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
						}
						if ( NOT_NULL ( tv ) && NOT_NULL ( sv ) ) {
							if ( tv->size() != sv->size() ) {
								return false;
							}

							for ( uint32_t ii = 0; ii < tv->size(); ii++ ) {
								const mutable_test_type * v1 = ( *sv ) [ii];
								const mutable_test_type * v2 = ( *tv ) [ii];
								if ( IS_NULL ( v1 ) ) {
									if ( NOT_NULL ( v2 ) ) {
										return false;
									}
									continue;
								}
								// Check inner type equals.
								bool r = v1->equals ( v2 );
								if ( !r ) {
									return false;
								}
							}
						}
					}



					return true;
				}


			};


			/**
			 * Generated code for type comparison utility for test_schema.
			 *
			 * Note:: Should not be modified as the changes will be lost when the code is re-generated.
			 */
			class compare_test_schema
			{
			public:
				static bool equals ( const test_schema * source, const mutable_test_schema * target )
				{
					if ( IS_NULL ( source ) ) {
						if ( IS_NULL ( target ) ) {
							return true;
						}
						return false;
					}

					{
						// Compare native field key
						const __native_type *nt = source->get_field_type ( "key" );
						CHECK_NOT_NULL ( nt );
						__base_datatype_io *th = nt->get_type_handler ( __record_mode::RM_READ );
						CHECK_NOT_NULL ( th );
						const void *tv = source->get_key();
						const void *sv = target->get_key();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
						}
						if ( NOT_NULL ( sv ) && NOT_NULL ( tv ) ) {
							bool r = th->compare ( tv, sv, __constraint_operator::EQ );
							if ( !r ) {
								return false;
							}
						}
					}

					{
						// Compare native field testLong
						const __native_type *nt = source->get_field_type ( "testLong" );
						CHECK_NOT_NULL ( nt );
						__base_datatype_io *th = nt->get_type_handler ( __record_mode::RM_READ );
						CHECK_NOT_NULL ( th );
						const void *tv = source->get_testLong();
						const void *sv = target->get_testLong();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
						}
						if ( NOT_NULL ( sv ) && NOT_NULL ( tv ) ) {
							bool r = th->compare ( tv, sv, __constraint_operator::EQ );
							if ( !r ) {
								return false;
							}
						}
					}

					{
						// Compare native field shortWithDefault
						const __native_type *nt = source->get_field_type ( "shortWithDefault" );
						CHECK_NOT_NULL ( nt );
						__base_datatype_io *th = nt->get_type_handler ( __record_mode::RM_READ );
						CHECK_NOT_NULL ( th );
						const void *tv = source->get_shortWithDefault();
						const void *sv = target->get_shortWithDefault();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
						}
						if ( NOT_NULL ( sv ) && NOT_NULL ( tv ) ) {
							bool r = th->compare ( tv, sv, __constraint_operator::EQ );
							if ( !r ) {
								return false;
							}
						}
					}

					{
						// Compare type testTypeRef
						const test_ref_type * s_ptr = source->get_testTypeRef();
						const mutable_test_ref_type * t_ptr = target->get_testTypeRef();
						if ( IS_NULL ( s_ptr ) ) {
							if ( IS_NULL ( t_ptr ) ) {
								return true;
							}
						}
						if ( NOT_NULL ( s_ptr ) && NOT_NULL ( t_ptr ) ) {
							bool r = compare_test_ref_type::equals ( s_ptr, t_ptr );
							if ( !r ) {
								return false;
							}
						}
					}

					{
						// Compare list instance testListRef
						const vector<test_type *> *sv = source->get_testListRef();
						const vector<mutable_test_type *> *tv = target->get_testListRef();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
						}
						if ( NOT_NULL ( tv ) && NOT_NULL ( sv ) ) {
							if ( tv->size() != sv->size() ) {
								return false;
							}

							for ( uint32_t ii = 0; ii < tv->size(); ii++ ) {
								const test_type * v1 = ( *sv ) [ii];
								const mutable_test_type * v2 = ( *tv ) [ii];
								if ( IS_NULL ( v1 ) ) {
									if ( NOT_NULL ( v2 ) ) {
										return false;
									}
									continue;
								}
								// Check inner type equals.
								bool r = compare_test_type::equals ( v1, v2 );
								if ( !r ) {
									return false;
								}
							}
						}
					}



					return true;
				}

			};


		}

	}

}

#endif // TEST_SCHEMA_H

