//
// Created by Subhabrata Ghosh on 18/11/16.
//

#ifndef REACTFS_RESOURCE_HELPER_H
#define REACTFS_RESOURCE_HELPER_H

#include "watergate/includes/filesystem_driver.h"
#include "watergate/src/core/control/dummy_resource.h"
#include "watergate/includes/resource_factory.h"

using namespace com::wookler::watergate::core;
using namespace com::wookler::watergate::core::io;

namespace com {
    namespace wookler {
        namespace watergate {
            namespace core {
                namespace ext {
                    class resource_helper {
                    public:
                        static void register_creators() {
                            dummy_resource_creator *dc = new dummy_resource_creator();
                            CHECK_ALLOC(dc, TYPE_NAME(dummy_resource_creator));
                            resource_factory::add_resource(dummy_resource::DUMMY_RESOURCE_CLASS, dc);

                            filesystem_driver_creator *fc = new filesystem_driver_creator();
                            CHECK_ALLOC(fc, TYPE_NAME(filesystem_driver_creator));
                            resource_factory::add_resource(fs_driver_constants::FS_RESOURCE_CLASS, fc);
                        }
                    };
                }
            }
        }
    }
}
#endif //REACTFS_RESOURCE_HELPER_H
