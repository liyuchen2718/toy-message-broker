#ifndef THREADSAFE_QUEUE_H_
#define THREADSAFE_QUEUE_H_
#include <queue>
#include <mutex>
#include <condition_variable>
#include <initializer_list>
 
 
 
class threadsafe_queue{
private:
 
    mutable std::mutex mut;
    mutable std::condition_variable data_cond;
    using queue_type = std::queue<std::string>;
    queue_type data_queue;
    threadsafe_queue()=default;
    threadsafe_queue(const threadsafe_queue&)=delete;
    threadsafe_queue& operator=(const threadsafe_queue&)=delete;
 

    static threadsafe_queue instance;
public:
    using value_type= typename queue_type::value_type;
    using container_type = typename queue_type::container_type;
    
    
    static threadsafe_queue& getInstance() {
        return instance;
    }
 

    
 
    void push(const value_type &new_value){
        std::lock_guard<std::mutex>lk(mut);
        data_queue.push(std::move(new_value));
        data_cond.notify_one();
    }
 
    value_type wait_and_pop(){
        std::unique_lock<std::mutex>lk(mut);
        data_cond.wait(lk,[this]{return !this->data_queue.empty();});
        auto value=std::move(data_queue.front());
        data_queue.pop();
        return value;
    }
 
    bool try_pop(value_type& value){
        std::lock_guard<std::mutex>lk(mut);
        if(data_queue.empty())
            return false;
        value=std::move(data_queue.front());
        data_queue.pop();
        return true;
    }
 
    auto empty() const->decltype(data_queue.empty()) {
        std::lock_guard<std::mutex>lk(mut);
        return data_queue.empty();
    }
 
    auto size() const->decltype(data_queue.size()){
        std::lock_guard<std::mutex>lk(mut);
        return data_queue.size();
    }
}; /* threadsafe_queue */



 


#endif /* THREADSAFE_QUEUE_H_ */